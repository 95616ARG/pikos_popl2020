#pragma once

#include <iomanip> // std::setprecision
#include <memory>
#include <unordered_map>

#include <ikos/core/fixpoint/concurrent_fixpoint_iterator.hpp>
#include <ikos/core/fixpoint/fwd_fixpoint_iterator.hpp>
#include <ikos/core/fixpoint/wpo.hpp>

#include <tbb/tbb.h>
#include <tbb/atomic.h>
#include <tbb/parallel_do.h>
#include <tbb/cache_aligned_allocator.h>
#include <tbb/tbb_allocator.h>
#include <tbb/concurrent_unordered_map.h>

#include <algorithm>
#include <vector>

namespace ikos {
namespace core {

/// \brief Interleaved concurrent forward fixpoint iterator
///
/// This class computes a fixpoint on a control flow graph.
template <typename GraphRef,
          typename AbstractValue,
          typename GraphTrait = GraphTraits<GraphRef>>
class InterleavedCFwdFixpointIterator
  : public ConcurrentForwardFixpointIterator<GraphRef, AbstractValue, GraphTrait> {
private:
  using NodeRef = typename GraphTrait::NodeRef;
  using WpoIdx = uint32_t;
  using InvariantTable = tbb::concurrent_unordered_map<NodeRef, AbstractValue>;
  using WpoT = Wpo<GraphRef, GraphTrait>;

public:
  class Cell;

private:
  GraphRef _cfg;
  NodeRef _entry;
  WpoT _wpo;
  InvariantTable _pre, _post;
  std::vector<Cell, tbb::cache_aligned_allocator<Cell>> _cells;

public:
  /// \brief Create an interleaved forward fixpoint iterator
  explicit InterleavedCFwdFixpointIterator(GraphRef cfg)
      : _cfg(cfg),
        _entry(GraphTrait::entry(cfg)),
        _wpo(cfg, false),
        _cells(_wpo.size()) {}

  /// \brief Deleted copy constructor
  InterleavedCFwdFixpointIterator(const InterleavedCFwdFixpointIterator&) = delete;

  /// \brief Move constructor
  InterleavedCFwdFixpointIterator(InterleavedCFwdFixpointIterator&&) = default;

  /// \brief Deleted copy assignment operator
  InterleavedCFwdFixpointIterator& operator=(const InterleavedCFwdFixpointIterator&) = delete;

  /// \brief Move assignment operator
  InterleavedCFwdFixpointIterator& operator=(InterleavedCFwdFixpointIterator&&) = default;

  /// \brief Get the control flow graph
  GraphRef cfg() const { return this->_cfg; }

  /// \brief Get the weak partial order of the graph
  const WpoT& wpo() const { return this->_wpo; }

private:
  /// \brief Set the invariant for the given node
  static void set(InvariantTable& table, NodeRef node, AbstractValue inv) {
    auto it = table.find(node);
    if (it != table.end()) {
      it->second = std::move(inv);
    } else {
      table.emplace(node, std::move(inv));
    }
  }

  /// \brief Set the pre invariant for the given node
  void set_pre(NodeRef node, AbstractValue inv) {
    this->set(this->_pre, node, std::move(inv));
  }

  /// \brief Set the post invariant for the given node
  void set_post(NodeRef node, AbstractValue inv) {
    this->set(this->_post, node, std::move(inv));
  }

  /// \brief Get the invariant for the given node
  static const AbstractValue& get(const InvariantTable& table, NodeRef node) {
    auto it = table.find(node);
    if (it != table.end()) {
      return it->second;
    } else {
      static AbstractValue bottom = AbstractValue::bottom();
      return bottom;
    }
  }

public:
  /// \brief Get the pre invariant for the given node
  const AbstractValue& pre(NodeRef node) const {
    return this->get(this->_pre, node);
  }

  /// \brief Get the post invariant for the given node
  const AbstractValue& post(NodeRef node) const {
    return this->get(this->_post, node);
  }

  /// \brief Extrapolate the new state after an increasing iteration
  ///
  /// This is called after each iteration of a cycle, until the fixpoint is
  /// reached. In order to converge, the widening operator must be applied.
  /// This method gives the user the ability to use different widening
  /// strategies.
  ///
  /// By default, it applies a join for the first iteration, and then the
  /// widening until it reaches the fixpoint.
  ///
  /// \param head Head of the cycle
  /// \param iteration Iteration number
  /// \param before Abstract value before the iteration
  /// \param after Abstract value after the iteration
  virtual AbstractValue extrapolate(NodeRef head,
                                    unsigned iteration,
                                    AbstractValue before,
                                    AbstractValue after) {
    ikos_ignore(head);
    if (iteration <= 1) {
      before.join_iter_with(after);
    } else {
      before.widen_with(after);
    }
    return before;
  }

  /// \brief Return the partition id of a node.
  virtual int partition(NodeRef node) {
    return 0;
  }

  /// \brief Return the maximum available parallelism.
  virtual size_t max_parallelism() {
    return 1;
  }

  /// \brief Check if the increasing iterations fixpoint is reached
  ///
  /// \param before Abstract value before the iteration
  /// \param after Abstract value after the iteration
  virtual bool is_increasing_iterations_fixpoint(const AbstractValue& before,
                                                 const AbstractValue& after) {
    return after.leq(before);
  }
  /// \brief Refine the new state after a decreasing iteration
  ///
  /// This is called after each iteration of a cycle, until the post fixpoint
  /// is reached. In order to converge, the narrowing operator must be applied.
  /// This method gives the user the ability to use different narrowing
  /// strategies.
  ///
  /// By default, it applies the narrowing until it reaches the post fixpoint.
  ///
  /// \param head Head of the cycle
  /// \param iteration Iteration number
  /// \param before Abstract value before the iteration
  /// \param after Abstract value after the iteration
  virtual AbstractValue refine(NodeRef head,
                               unsigned iteration,
                               AbstractValue before,
                               AbstractValue after) {
    ikos_ignore(head);
    ikos_ignore(iteration);
    before.narrow_with(after);
    return before;
  }

  /// \brief Check if the decreasing iterations fixpoint is reached
  ///
  /// \param before Abstract value before the iteration
  /// \param after Abstract value after the iteration
  virtual bool is_decreasing_iterations_fixpoint(const AbstractValue& before,
                                                 const AbstractValue& after) {
    return before.leq(after);
  }

  /// \brief Notify the beginning of the analysis of a cycle
  ///
  /// This method is called before analyzing a cycle.
  virtual void notify_enter_cycle(NodeRef head) { ikos_ignore(head); }

  /// \brief Notify the beginning of an iteration on a cycle
  ///
  /// This method is called for each iteration on a cycle.
  virtual void notify_cycle_iteration(NodeRef head,
                                      unsigned iteration,
                                      FixpointIterationKind kind) {
    ikos_ignore(head);
    ikos_ignore(iteration);
    ikos_ignore(kind);
  }

  /// \brief Notify the end of the analysis of a cycle
  ///
  /// This method is called after reaching a fixpoint on a cycle.
  virtual void notify_leave_cycle(NodeRef head) { ikos_ignore(head); }

  enum class CellType {
    LOOP_HEADER_CELL,
    LOOP_EXIT_CELL,
    PLAIN_CELL,
  };

  /// \brief TBB Cell that is going to be executed.
  /// There is one cell for each WPO element.
  class Cell {
  public:
    typedef std::vector<Cell*, tbb::cache_aligned_allocator<Cell*>> CellPtrVector;

    CellType _cell_type;
    NodeRef _node; // Only relevant for plain and header cells.
    WpoIdx _idx;
    InterleavedCFwdFixpointIterator* _it;
    // These are only relevant for plain and header cells.
    CellPtrVector _data_predecessors;
    // These are only relevant for header cells.
    FixpointIterationKind _iteration_kind;
    uint32_t _iteration_count;
    // This is only relevant for exit cells.
    Cell* _header_cell;
    // Number of executed control predecessors.
    // A cell is executed when it reaches the number of all control predecessors.
    tbb::atomic<int> _executed_control_preds;
    int _num_control_preds;
    CellPtrVector _control_successors;
    // Partition for parallelism.
    int _partition;

    void dump(std::ostream& o) const {
      o << "Idx " << _idx;
      o << (_cell_type == CellType::PLAIN_CELL
                ? " plain"
                : (_cell_type == CellType::LOOP_HEADER_CELL ? " head"
                                                            : " exit"));
      o << " " << size();
    }

    size_t size() const {
      if (_cell_type == CellType::PLAIN_CELL) {
        return _node->size() + 1;
      } else if (_cell_type == CellType::LOOP_HEADER_CELL) {
        WpoIdx exit = _it->_wpo.get_exit_of_head(_idx);
        auto component_size = _it->_wpo.get_size(exit);
        return (_node->size() + 10) * component_size;
      } else {
        auto component_size = _it->_wpo.get_size(_idx);
        return (_header_cell->_node->size() + 5) * component_size;
      }
      // return _cell_type == CellType::PLAIN_CELL ? _node->size() : 100;
    }

    const CellPtrVector& apply_header_rule() {
      if (_iteration_count == 0) {
        // Initialization of loop join node.
        AbstractValue pre = AbstractValue::bottom();
        // Collect invariants from incoming edges.
        for (auto pred : _data_predecessors) {
          NodeRef pred_node = pred->_node;
          // Only the input nodes are relevant.
          if (_it->_wpo.is_from_outside(_node, pred_node)) {
            pre.join_with(std::move(_it->analyze_edge(pred_node,
                                                      _node,
                                                      _it->post(pred_node))));
          }
        }
        _iteration_count = 1;
        _it->set_pre(_node, std::move(pre));
      }
      _it->set_post(_node, std::move(_it->analyze_node(_node, _it->pre(_node))));
      return _control_successors;
    }

    // Returns true if the loop is stabilized, false otherwise.
    bool check_stabilization_and_widen_in_join_node() {
      // Input nodes.
      AbstractValue new_pre_in = AbstractValue::bottom();
      // Back nodes.
      AbstractValue new_pre_back = AbstractValue::bottom();
      for (auto pred : _data_predecessors) {
        NodeRef pred_node = pred->_node;
        if (_it->_wpo.is_from_outside(_node, pred_node)) {
          new_pre_in.join_with(
              std::move(_it->analyze_edge(pred_node, _node, _it->post(pred_node))));
        } else {
          new_pre_back.join_with(
              std::move(_it->analyze_edge(pred_node, _node, _it->post(pred_node))));
        }
      }

      new_pre_in.join_loop_with(std::move(new_pre_back));
      AbstractValue new_pre(std::move(new_pre_in));

      AbstractValue pre(_it->pre(_node));

      if (_iteration_kind == FixpointIterationKind::Increasing) {
        // Increasing iteration with widening.
        if (_it->is_increasing_iterations_fixpoint(pre, new_pre)) {
          // Post-fixpoint reached.
          // Use this iteration as a decreasing iteration.
          _iteration_kind = FixpointIterationKind::Decreasing;
          _iteration_count = 1;
        } else {
          _it->set_pre(_node, std::move(_it->extrapolate(_node,
                                                         _iteration_count,
                                                         std::move(pre),
                                                         std::move(new_pre))));
          _iteration_count++;
          return false; // Not stabilized.
        }
      }

      if (_iteration_kind == FixpointIterationKind::Decreasing) {
        // Decreasing iteration with narrowing.
        new_pre = std::move(_it->refine(_node,
                                        _iteration_count,
                                        pre,
                                        std::move(new_pre)));
        if (_it->is_decreasing_iterations_fixpoint(pre, new_pre)) {
          // No more refinement possible.
          _it->set_pre(_node, std::move(new_pre));
          _iteration_kind = FixpointIterationKind::Increasing;
          _iteration_count = 0;
          return true; // Stabilized.
        } else {
          _it->set_pre(_node, std::move(new_pre));
          _iteration_count++;
          return false; // Not stabilized.
        }
      }
    }

    const CellPtrVector& apply_exit_rule() {
      bool stabilized = _header_cell->check_stabilization_and_widen_in_join_node();
      if (stabilized) {
        return _control_successors;
      } else {
        // Not stabilized.
        _executed_control_preds = 0;
        set_executed_control_preds_for_component();
        return _header_cell->apply_header_rule();
      }
    }

    const CellPtrVector& apply_plain_rule() {
      AbstractValue pre = AbstractValue::bottom();
      if (_node == _it->_entry) {
        pre = _it->pre(_node);
      }
      // Collect invariants from incoming edges.
      for (auto pred : _data_predecessors) {
        pre.join_with(std::move(_it->analyze_edge(pred->_node, _node, _it->post(pred->_node))));
      }
      _it->set_pre(_node, pre);
      _it->set_post(_node, std::move(_it->analyze_node(_node, std::move(pre))));
      return _control_successors;
    }

    // Apply the rules based on the cell types.
    const CellPtrVector& apply_rule() {
      switch (_cell_type) {
      case CellType::PLAIN_CELL:
        return apply_plain_rule();
      case CellType::LOOP_HEADER_CELL:
        return apply_header_rule();
      case CellType::LOOP_EXIT_CELL:
        return apply_exit_rule();
      default:
        ikos_assert_msg(false, "Unknown Cell Type");
      }
    }

    // Set the executed_control_preds for component.
    void set_executed_control_preds_for_component() {
      for (auto p : _it->_wpo.get_num_outer_preds(_idx)) {
        _it->_cells[p.first]._executed_control_preds = p.second;
      }
    }

    Cell()
      : _cell_type(CellType::PLAIN_CELL),
        _iteration_kind(FixpointIterationKind::Increasing),
        _iteration_count(0),
        _executed_control_preds(0),
        _partition(0) {}

    // Reset all values. Useful when using an object pool.
    void reset() {
      _cell_type = CellType::PLAIN_CELL;
      _iteration_kind = FixpointIterationKind::Increasing;
      _iteration_count = 0;
      _executed_control_preds = 0;
      _partition = 0;
      _data_predecessors.clear();
      _control_successors.clear();
    }
  };

  // "parallel_do" uses this to run cells and schedule control successors.
  class Body {
  public:
    Body() {};

    // Following signatures are required by "parallel_do".
    typedef Cell* argument_type;

    void operator()(argument_type c,
                    tbb::parallel_do_feeder<argument_type>& feeder) const {
      std::deque<argument_type> wl;
      wl.push_back(c);

      while (!wl.empty()) {
        auto cell = wl.front();
        wl.pop_front();

        // Run the cell.
        const auto& succs = cell->apply_rule();
        // Reset the executed_control_preds.
        cell->_executed_control_preds = 0;
        // Notify the control successors.
        for (Cell* succ : succs) {
          int curr = succ->_executed_control_preds.fetch_and_increment();
          if (curr + 1 < succ->_num_control_preds) {
            continue; // Nothing to do.
          }
          // Schedule the successor.
          bool reuse_task = true;
          if (wl.empty()) {
            reuse_task = true;
          } else if (succ->_cell_type == CellType::LOOP_EXIT_CELL) {
            reuse_task = true;
          } else if (succ->_partition > 0 && cell->_partition != succ->_partition) {
            reuse_task = false;
          }
          if (reuse_task) {
            wl.push_back(succ);
          } else {
            feeder.add(succ);
          }
        }
      }
    }
  };

  /// \brief Compute the fixpoint with the given initial abstract value
  void run(AbstractValue init) {
    auto size = _wpo.size();
    std::vector<Cell*> entries;
    std::unordered_map<NodeRef, WpoIdx> node_to_idx; // Temporary.

    // WPO2Cells.
    // Populate TBB cells for WPO elements.
    // WPO just talks about the order. Cells are the ones actually runnable.
    for (WpoIdx idx = 0; idx < size; ++idx) {
      Cell& c = _cells[idx];

      // WpoIdx.
      c._idx = idx;
      if (_wpo.is_head(idx) || _wpo.is_plain(idx)) {
        // NodeRef.
        c._node = _wpo.get_node(idx);
        node_to_idx[c._node] = idx;

        // Partition Id (path-based heuristic).
        c._partition = this->partition(c._node);
      }
      // Type.
      if (_wpo.is_head(idx)) {
        c._cell_type = CellType::LOOP_HEADER_CELL;
      } else if (_wpo.is_plain(idx)) {
        c._cell_type = CellType::PLAIN_CELL;
      } else {
        c._cell_type = CellType::LOOP_EXIT_CELL;
      }
      // Counter.
      c._num_control_preds = _wpo.get_num_preds(idx);
      // Iterator.
      c._it = this;
      // Initialize pre for entry.
      if (idx == _wpo.get_entry()) {
        this->set_pre(c._node, std::move(init));
        entries.push_back(&c);
      }
    }
    // Put order between TBB cells.
    for (Cell& c : _cells) {
      for (auto s : _wpo.get_successors(c._idx)) {
        // Scheduling constraints, control_successors.
        Cell& succ = _cells[s];
        c._control_successors.push_back(&succ);
      }
      if (c._cell_type == CellType::LOOP_EXIT_CELL) {
        // Stabilization constraint, header_cell.
        c._header_cell = &_cells[_wpo.get_head_of_exit(c._idx)];
      } else {
        NodeRef& node = c._node;
        for (auto it = GraphTrait::predecessor_begin(node),
                  et = GraphTrait::predecessor_end(node);
             it != et;
             ++it) {
          // Dependencies, data_predecessors.
          WpoIdx pred_idx = node_to_idx[*it];
          Cell& pred = _cells[pred_idx];
          c._data_predecessors.push_back(&pred);
        }
      }
    }
    node_to_idx.clear(); // No longer needed.

    // Run the cells.
    tbb::parallel_do(entries.begin(), entries.end(), Body());

    // Populate the pre and post maps.
    for (auto& c : _cells) {
      if (c._cell_type != CellType::LOOP_EXIT_CELL) {
        this->process_pre(c._node, pre(c._node));
        this->process_post(c._node, post(c._node));
      }
    }
    return;
  }

  /// \brief Clear the pre invariants
  void clear_pre() { this->_pre.clear(); }

  /// \brief Clear the post invariants
  void clear_post() { this->_post.clear(); }

  /// \brief Clear the current fixpoint
  void clear() {
    this->_pre.clear();
    this->_post.clear();
  }

  /// \brief Destructor
  ~InterleavedCFwdFixpointIterator() override = default;
}; // end class InterleavedCFwdFixpointIterator

} // end namespace core
} // end namespace ikos
