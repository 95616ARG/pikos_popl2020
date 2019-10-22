/*******************************************************************************
 *
 * \file
 * \brief FixpointProfileAnalysis implementation used by the analysis
 *
 * Author: Thomas Bailleux
 *
 * Contact: ikos@lists.nasa.gov
 *
 * Notices:
 *
 * Copyright (c) 2018-2019 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Disclaimers:
 *
 * No Warranty: THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY OF
 * ANY KIND, EITHER EXPRESSED, IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL CONFORM TO SPECIFICATIONS,
 * ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * OR FREEDOM FROM INFRINGEMENT, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL BE
 * ERROR FREE, OR ANY WARRANTY THAT DOCUMENTATION, IF PROVIDED, WILL CONFORM TO
 * THE SUBJECT SOFTWARE. THIS AGREEMENT DOES NOT, IN ANY MANNER, CONSTITUTE AN
 * ENDORSEMENT BY GOVERNMENT AGENCY OR ANY PRIOR RECIPIENT OF ANY RESULTS,
 * RESULTING DESIGNS, HARDWARE, SOFTWARE PRODUCTS OR ANY OTHER APPLICATIONS
 * RESULTING FROM USE OF THE SUBJECT SOFTWARE.  FURTHER, GOVERNMENT AGENCY
 * DISCLAIMS ALL WARRANTIES AND LIABILITIES REGARDING THIRD-PARTY SOFTWARE,
 * IF PRESENT IN THE ORIGINAL SOFTWARE, AND DISTRIBUTES IT "AS IS."
 *
 * Waiver and Indemnity:  RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST
 * THE UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL
 * AS ANY PRIOR RECIPIENT.  IF RECIPIENT'S USE OF THE SUBJECT SOFTWARE RESULTS
 * IN ANY LIABILITIES, DEMANDS, DAMAGES, EXPENSES OR LOSSES ARISING FROM SUCH
 * USE, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING FROM,
 * RECIPIENT'S USE OF THE SUBJECT SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD
 * HARMLESS THE UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS,
 * AS WELL AS ANY PRIOR RECIPIENT, TO THE EXTENT PERMITTED BY LAW.
 * RECIPIENT'S SOLE REMEDY FOR ANY SUCH MATTER SHALL BE THE IMMEDIATE,
 * UNILATERAL TERMINATION OF THIS AGREEMENT.
 *
 ******************************************************************************/

#include <ikos/core/fixpoint/wto.hpp>
#include <ikos/core/fixpoint/wpo.hpp>

#include <ikos/ar/semantic/statement.hpp>

#include <ikos/analyzer/analysis/fixpoint_profile.hpp>
#include <ikos/analyzer/support/cast.hpp>
#include <ikos/analyzer/util/demangle.hpp>
#include <ikos/analyzer/util/log.hpp>
#include <ikos/analyzer/util/progress.hpp>
#include <ikos/analyzer/analysis/pointer/value.hpp>
#include <ikos/ar/verify/type.hpp>


namespace ikos {
namespace analyzer {

namespace {

class FixpointProfileWtoVisitor
    : public core::WtoComponentVisitor< ar::Code* > {
private:
  using WtoVertexT = core::WtoVertex< ar::Code* >;
  using WtoCycleT = core::WtoCycle< ar::Code* >;

private:
  llvm::DenseMap< ar::BasicBlock*, std::unique_ptr< core::MachineInt > >*
      _collector;
  std::unordered_set< ar::BasicBlock*>* _spawn_set;
  
public:
  /// \brief Default constructor
  explicit FixpointProfileWtoVisitor(
      llvm::DenseMap< ar::BasicBlock*, std::unique_ptr< core::MachineInt > >*
          collector) {
    this->_collector = collector;
  }

  /// \brief Default copy constructor
  FixpointProfileWtoVisitor(const FixpointProfileWtoVisitor&) = delete;

  /// \brief Default move constructor
  FixpointProfileWtoVisitor(FixpointProfileWtoVisitor&&) = delete;

  /// \brief Delete copy assignment operator
  FixpointProfileWtoVisitor& operator=(const FixpointProfileWtoVisitor&) =
      delete;

  /// \brief Delete move assignment operator
  FixpointProfileWtoVisitor& operator=(FixpointProfileWtoVisitor&&) = delete;

  /// \brief Destructor
  ~FixpointProfileWtoVisitor() override = default;

  void visit(const WtoVertexT&) override {}

  void visit(const WtoCycleT& cycle) override {
    auto head = cycle.head();

    if (head->num_successors() > 1) {
      auto successor = *(head->successor_begin());
      if (auto constant = this->extract_constant(successor)) {
        this->_collector->try_emplace(head,
                                      std::make_unique< ar::MachineInt >(
                                          *constant));
      }
    }

    for (auto it = cycle.begin(), et = cycle.end(); it != et; ++it) {
      it->accept(*this);
    }
  }

  boost::optional< ar::MachineInt > extract_constant(ar::BasicBlock* bb) const {
    if (bb->empty()) {
      return boost::none;
    }

    // we have a comparison, check if there is a constant
    if (auto cmp = dyn_cast< ar::Comparison >(bb->front())) {
      ar::IntegerConstant* constant = nullptr;
      bool cst_left;

      if (cmp->left()->is_integer_constant()) {
        constant = cast< ar::IntegerConstant >(cmp->left());
        cst_left = true;
      } else if (cmp->right()->is_integer_constant()) {
        constant = cast< ar::IntegerConstant >(cmp->right());
        cst_left = false;
      } else {
        return boost::none;
      }

      ar::MachineInt value = constant->value();
      ar::MachineInt one(1, value.bit_width(), value.sign());
      bool overflow = false;

      // check if the comparison is <= or >=
      if (cmp->predicate() == ar::Comparison::UIGE ||
          cmp->predicate() == ar::Comparison::SIGE) {
        if (cst_left) {
          // case `cst >= var` <=> `cst + 1 > var`
          value = add(value, one, overflow);
        } else {
          // case `var >= cst` <=> `var > cst - 1`
          value = sub(value, one, overflow);
        }
      } else if (cmp->predicate() == ar::Comparison::UILE ||
                 cmp->predicate() == ar::Comparison::SILE) {
        if (cst_left) {
          // case `cst <= var` <=> `cst - 1 < var`
          value = sub(value, one, overflow);
        } else {
          // case `var <= cst` <=> `var < cst + 1`
          value = add(value, one, overflow);
        }
      }
      if (overflow) {
        return boost::none;
      }
      return value;
    } else {
      return boost::none;
    }
  }

}; // end class FixpointProfileWtoVisitor

} // end anonymous namespace

void FixpointProfileAnalysis::run() {
  auto bundle = this->_ctx.bundle;

  // Setup a progress logger
  std::unique_ptr< ProgressLogger > progress =
      make_progress_logger(_ctx.opts.progress,
                           LogLevel::Info,
                           /* num_tasks = */
                           std::count_if(bundle->function_begin(),
                                         bundle->function_end(),
                                         [](ar::Function* fun) {
                                           return fun->is_definition();
                                         }));
  ScopeLogger scope(*progress);

  for (auto it = bundle->function_begin(), et = bundle->function_end();
       it != et;
       ++it) {
    ar::Function* fun = *it;
    if (fun->is_definition()) {
      progress->start_task("Running fixpoint profile analysis on function '" +
                           demangle(fun->name()) + "'");
      if (auto profile = this->analyze_function(fun)) {
        this->_map.try_emplace(fun, std::move(profile));
      }
    }
  }
}



void FixpointProfileAnalysis::dump(std::ostream& o) const {
  for (const auto& item : this->_map) {
    o << "function " << item.first->name() << ":\n";
    item.second->dump(o);
    o << "\n";
  }
}

namespace {

using WpoIdx = uint32_t;

size_t compute_function_size(Context& ctx, ar::Function* fun, std::vector<std::unordered_map< ar::Function*, size_t>>& function_size_map, int cs);

size_t compute_block_size(Context& ctx, ar::BasicBlock* node, std::vector<std::unordered_map< ar::Function*, size_t>>& function_size_map, int cs) {
  size_t block_size = node->num_statements();
  // Account for function calls.
  for (auto it = node->begin(); it != node->end(); it++) {
    if (!isa< ar::Call >(*it)) {
      continue;
    }
    auto call = cast< ar::CallBase >(*it);
    ar::Value* called = call->called();
    if (auto ptr = dyn_cast< ar::InternalVariable >(called)) {
      // Estimate the size of an indirect call through a function pointer
      block_size += 522;
    }
    if (auto cst = dyn_cast< ar::FunctionPointerConstant >(called)) {
      MemoryLocation* mem =
          ctx.mem_factory->get_function(cst->function());
      if (!isa< FunctionMemoryLocation >(mem)) {
        // Not a call to a function memory location
        continue;
      }

      ar::Function* callee = cast< FunctionMemoryLocation >(mem)->function();

      if (!ar::TypeVerifier::is_valid_call(call, callee->type())) {
        // Ill-formed function call
        // This could be because of an imprecision of the pointer analysis.
        continue;
      }

      if (callee->is_declaration()) {
        continue;
      }
      // Estimate for function call size.
      if (cs == 1) {
        block_size += 1;
      } else {
        block_size += compute_function_size(ctx, callee, function_size_map, cs < 1 ? cs : cs-1);
      }
    }
  }
  return block_size;
}

size_t compute_function_size(Context& ctx, ar::Function* fun, std::vector<std::unordered_map< ar::Function*, size_t>>& function_size_map, int cs) {
  auto it = function_size_map[cs].find(fun);
  if (it != function_size_map[cs].end()) {
    return it->second == 0 ? 1 : it->second;
  }
  if (!fun->is_definition()) {
      function_size_map[cs].insert(std::make_pair(fun, 1));
      return 1;
  }
  // Mark that the function size is being computed. 
  auto p = function_size_map[cs].insert(std::make_pair(fun, 0));
  size_t fun_size = 1;
  for (ar::BasicBlock* bb : *fun->body()) {
    fun_size += compute_block_size(ctx, bb, function_size_map, cs);
  }
  p.first->second = fun_size;
  return fun_size;
}

  
size_t compute_longest_path(int color,
			                      Context& ctx,
                            const core::Wpo< ar::Code* >& wpo,
                            size_t weighted_size,
                            const std::vector< WpoIdx >& topological_sort,
                            std::vector<std::unordered_map< ar::Function*, size_t >>& function_size_map,
                            std::unordered_map< WpoIdx, int >* partition,
                            int cs) {
  size_t size = wpo.size();
  if (partition->size() >= size) return true; /* done */
  
  std::unordered_map< WpoIdx, size_t > node_to_path_length;
  for (auto node : topological_sort) {
    if (partition->find(node) != partition->end()) {
        continue;
    }
    size_t max = 0;
    for (auto p : wpo.get_predecessors(node)) {
      // Predecessor already partitioned, so ignore.
      if (partition->find(p) != partition->end()) {
        continue;
      }
      auto temp = node_to_path_length[p];
      if (temp > max) {
        max = temp;
      }
    }
    size_t node_size = wpo.is_exit(node) ? 10 : compute_block_size(ctx, wpo.get_node(node), function_size_map, cs) + 1;
    node_to_path_length[node] = max + node_size;
  }

  WpoIdx longest_path_id;
  size_t longest_path_length = 0;
  for (size_t i = 0; i < wpo.size(); i++) {
    if (partition->find(i) != partition->end()) {
        continue;
    }
    size_t temp = node_to_path_length[i];
    if (temp >= longest_path_length) {
      longest_path_length = temp;
      longest_path_id = i;
    }
  }

  // std::cerr << cs << " Longest path length " << longest_path_length << " Id "
  //           << longest_path_id << " Weighted size " << weighted_size
  //           << std::endl;

  // Longest path is too short.             
  if (longest_path_length <= 500) {
    return true /* done */;
  }

  // Traverse backwards from the longest_path_id, marking nodes.
  std::deque<WpoIdx> wl;
  wl.push_back(longest_path_id);
  size_t nodes_in_longest_path = 0; // Number of nodes in longest path.
  while (!wl.empty()) {
    WpoIdx node = wl.front();
    wl.pop_front();
    nodes_in_longest_path++;
    partition->insert(std::make_pair(node, color)); // Longest path.
    WpoIdx max_pred_id;
    size_t max_pred_length = 0;
    if (wpo.get_num_preds(node) == 0) {
      continue;
    }
    bool found = false;
    for (auto p : wpo.get_predecessors(node)) {
      if (partition->find(p) != partition->end()) {
        continue;
      }
      found = true;
      size_t temp = node_to_path_length[p];
      if (temp >= max_pred_length) {
        max_pred_id = p;
        max_pred_length = temp;
      }
    }
    if (found)  wl.push_back(max_pred_id);
  }

  return false; /* not done */
}

  size_t compute_topological_sort(Context& ctx, const core::Wpo< ar::Code*> &wpo,
				  std::vector<std::unordered_map< ar::Function*, size_t >>& function_size_map,
				  std::vector<WpoIdx>* topological_sort, int cs) {
  std::unordered_map< WpoIdx, size_t > node_to_pred_count;
  std::deque< WpoIdx > wl;
  size_t size = wpo.size();
  // Find entry and add to worklist.
  for (size_t i = 0; i < size; i++) {
    if (0 == wpo.get_num_preds(i)) {
      wl.push_back(i);
    }
  }
  size_t weighted_size = 0;
  // While worklist not empty.
  while (!wl.empty()) {
    WpoIdx node = wl.front();
    wl.pop_front();
    topological_sort->push_back(node);
    size_t node_size = wpo.is_exit(node) ? 10 : compute_block_size(ctx, wpo.get_node(node), function_size_map, cs) + 1;
    weighted_size += node_size;
    // Process successors.
    for (auto s : wpo.get_successors(node)) {
      node_to_pred_count[s]++;
      if (node_to_pred_count[s] == wpo.get_num_preds(s)) {
        wl.push_back(s);
      }
    }
  }
  return weighted_size;
}

  void compute_partition(Context& ctx, const core::Wpo< ar::Code*> &wpo, 
                         std::vector<tbb::concurrent_unordered_map< ar::BasicBlock*, int >>* block_partition,
                         std::vector<size_t>* number_of_partitions) {
  // Compute topological sort of the nodes.
  /*
  std::vector<WpoIdx> topological_sort;
  std::unordered_map< ar::Function*, size_t> function_size_map;  
  auto weighted_size = compute_topological_sort(ctx, wpo, &function_size_map, &topological_sort, 0);
  std::unordered_map< WpoIdx, int> partition;

  auto size = wpo.size();
  bool done = false;
  for (int i=1; !done; i++)  {
    done = compute_longest_path(i, ctx, wpo, weighted_size, topological_sort, &function_size_map, &partition);
  }
  for (auto p : partition) {
    if (wpo.is_exit(p.first)) continue;
    block_partition[0]->insert(std::make_pair(wpo.get_node(p.first), p.second));
  }
  */
  std::vector<std::unordered_map< ar::Function*, size_t>> function_size_map(ctx.opts.context_sensitivity+1);
  number_of_partitions->resize(ctx.opts.context_sensitivity+1);
  std::vector<WpoIdx> topological_sort;
  size_t weighted_size;

  std::vector<int> context_sensitivity;
  if (ctx.opts.context_sensitivity == 0) {
    context_sensitivity.push_back(0);
    number_of_partitions->at(0) = 0;
    weighted_size = compute_topological_sort(ctx, wpo, function_size_map, &topological_sort, 0);
  } else {
    for (int cs=1; cs <= ctx.opts.context_sensitivity; cs++) {
      context_sensitivity.push_back(cs);
      number_of_partitions->at(cs) = 0;
      weighted_size = compute_topological_sort(ctx, wpo, function_size_map, &topological_sort, 1);
    }
  }

  for (auto cs : context_sensitivity) {
    std::unordered_map< WpoIdx, int> partition;

    auto size = wpo.size();
    bool done = false;
    for (int i=1; !done; i++) {
      done = compute_longest_path(i, ctx, wpo, weighted_size, topological_sort, function_size_map, &partition, cs);
      if (!done) number_of_partitions->at(cs)++;
    }
    if (partition.size() < size) number_of_partitions->at(cs)++; // Account for zero partition.
    for (auto p : partition) {
      if (wpo.is_exit(p.first)) continue;
      block_partition->at(cs).insert(std::make_pair(wpo.get_node(p.first), p.second));
    }
  }
}


  
}  // namespace

std::unique_ptr< FixpointProfile > FixpointProfileAnalysis::analyze_function(
    ar::Function* fun) {
  if (!fun->is_definition()) {
    return nullptr;
  }

  std::unique_ptr< FixpointProfile > profile(new FixpointProfile(fun, _ctx.opts.context_sensitivity+1));
  FixpointProfileWtoVisitor visitor(&profile->_widening_hints);
  core::Wto< ar::Code* > wto(fun->body());
  wto.accept(visitor);
  core::Wpo< ar::Code* > wpo(fun->body(), false);
  //std::cerr << fun->name() << std::endl;
  compute_partition(this->_ctx, wpo, &profile->_partition, &profile->_number_of_partitions);

  // Iterate over basic blocks and identify blocks that should be spawned
  // by the concurrent fixpoint iterator.
  // for (ar::BasicBlock* node : *fun->body()) {
  //   if (is_spawnable(node)) {
  //     profile->_spawn_set.insert(node);
  //   }
  // }

  if (!profile->empty()) {
    return profile;
  } else {
    return nullptr;
  }
}

bool FixpointProfileAnalysis::is_spawnable(ar::BasicBlock* node) {
  // if (node->size() > 40)
  //   return true;

  for (auto it = node->begin(); it != node->end(); it++) {
    if (!isa< ar::Call >(*it)) {
      continue;
    }
    auto call = cast< ar::CallBase >(*it);
    ar::Value* called = call->called();
    if (auto ptr = dyn_cast< ar::InternalVariable >(called)) {
      // Indirect call through a function pointer
      //std::cerr << "Indirect function pointer spawnable " << std::endl;
      return true;
    }
    if (auto cst = dyn_cast< ar::FunctionPointerConstant >(called)) {
      MemoryLocation* mem =
          this->_ctx.mem_factory->get_function(cst->function());
      if (!isa< FunctionMemoryLocation >(mem)) {
        // Not a call to a function memory location
        continue;
      }

      ar::Function* callee = cast< FunctionMemoryLocation >(mem)->function();

      if (!ar::TypeVerifier::is_valid_call(call, callee->type())) {
        // Ill-formed function call
        // This could be because of an imprecision of the pointer analysis.
        continue;
      }

      if (callee->is_declaration()) {
        continue;
      }
      // Non-trivial function
      if (callee->body()->num_blocks() > 10) {
        //std::cerr << callee->name() << " spawnable " << callee->body()->size()
        //          << std::endl;
        return true;
      }
    }
  }
  return false;
}

boost::optional< const FixpointProfile& > FixpointProfileAnalysis::profile(
    ar::Function* fun) const {
  auto it = this->_map.find(fun);
  if (it != this->_map.end()) {
    return *(it->second);
  } else {
    return boost::none;
  }
}

boost::optional< const core::MachineInt& > FixpointProfile::widening_hint(
    ar::BasicBlock* bb) const {
  auto it = this->_widening_hints.find(bb);
  if (it == this->_widening_hints.end()) {
    return boost::none;
  } else {
    return *it->second;
  }
}

  
bool FixpointProfile::is_spawnable(ar::BasicBlock* bb) const {
  return this->_spawn_set.find(bb) != this->_spawn_set.end();
}

int FixpointProfile::partition(ar::BasicBlock* bb, int cs) const {
  auto it = this->_partition[cs].find(bb);
  if (it == this->_partition[cs].end()) {
    return 0;
  } else {
    return it->second;
  }
}

size_t FixpointProfile::number_of_partitions(int cs) const {
  if (cs >=0 && cs < _number_of_partitions.size()) {
    return _number_of_partitions[cs];
  }
  return 1;
}
  

bool FixpointProfile::empty() const {
  return this->_widening_hints.empty() && this->_spawn_set.empty() && this->_partition.empty();
}

void FixpointProfile::dump(std::ostream& o) const {
  for (const auto& item : this->_widening_hints) {
    o << " • ";
    item.first->dump(o);
    o << ": " << *item.second << "\n";
  }
}

} // namespace analyzer
} // end namespace ikos
