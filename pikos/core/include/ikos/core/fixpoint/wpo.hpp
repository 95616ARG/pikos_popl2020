/*******************************************************************************
 *
 * \file
 * \brief Construction and management of weak partial orders (WPOs).
 *
 * Author: Sung Kook Kim, Aditya V. Thakur
 *
 * Contact: {sklkim, avthakur}@ucdavis.edu
 *
 * Notices:
 *
 * Copyright (c) 2011-2019 United States Government as represented by the
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
#pragma once

#include <deque>
#include <functional>
#include <future>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <utility>
#include <vector>
#include <queue>
#include <stack>
#include <tuple>
#include <boost/pending/disjoint_sets.hpp>

#include <ikos/core/number/bound.hpp>
#include <ikos/core/semantic/dumpable.hpp>
#include <ikos/core/semantic/graph.hpp>

#include <tbb/concurrent_unordered_map.h>

namespace ikos {
namespace core {

// Forward declarations
template <typename GraphRef, typename GraphTrait>
class Wpo;

template <typename GraphRef, typename GraphTrait>
class WpoNode;

namespace wpo_impl {

template <typename GraphRef, typename GraphTrait>
class WpoBuilder;

} // end namespace wpo_impl

/// \brief Node of a weak partial ordering.
/// This is either head, plain, or exit.
template <typename GraphRef, typename GraphTrait = GraphTraits<GraphRef>>
class WpoNode final {
public:
  enum class Type { Plain, Head, Exit };

private:
  using NodeRef = typename GraphTrait::NodeRef;
  // Uses index of an array to id Wpo nodes.
  using WpoIdx = uint32_t;

private:
  NodeRef _node;
  Type _type;

  /// \brief Successors of scheduling constraints.
  std::set<WpoIdx> _successors;
  /// \brief Predecessors of scheduling constraints.
  std::set<WpoIdx> _predecessors;
  /// \brief Size of maximal SCC with this as its header.
  uint32_t _size;
  /// \brief Number of outer predecessors w.r.t. the component (for exits only)
  std::unordered_map<WpoIdx, uint32_t> _num_outer_preds;

public:
  WpoNode(const NodeRef& node,
          Type type,
          uint32_t size)
    : _node(node), _type(type), _size(size) {}

public:
  /// \brief Return the GraphRef for this node
  const NodeRef& get_node() const { return _node; }

  /// \brief Check the type of this node
  bool is_plain() const { return _type == Type::Plain; }
  bool is_head() const { return _type == Type::Head; }
  bool is_exit() const { return _type == Type::Exit; }

  /// \brief Get successors.
  const std::set<WpoIdx>& get_successors() const {
    return _successors;
  }

  /// \brief Get predecessors.
  const std::set<WpoIdx>& get_predecessors() const {
    return _predecessors;
  }

  /// \brief Get number of predecessors.
  uint32_t get_num_preds() const {
    return _predecessors.size();
  }

  /// \brief Get number of outer predecessors w.r.t. the component (for exits only).
  const std::unordered_map<WpoIdx, uint32_t>& get_num_outer_preds() const {
    ikos_assert_msg(_type == Type::Exit,
                    "Undefined operation for a non-exit node.");
    return _num_outer_preds;
  }

  /// \brief Get size of the SCC.
  uint32_t get_size() const {
    return _size;
  }

private:
  /// \brief Add successor.
  void add_successor(WpoIdx idx) {
    _successors.insert(idx);
  }

  /// \brief Add predecessor.
  void add_predecessor(WpoIdx idx) {
    _predecessors.insert(idx);
  }

  /// \brief Check if the given node is a successor.
  bool is_successor(WpoIdx idx) {
    return _successors.find(idx) != _successors.end();
  }

  /// \brief Increment the number of outer predecessors.
  void inc_num_outer_preds(WpoIdx idx) {
    ikos_assert_msg(_type == Type::Exit,
                    "Trying to add outer preds info to a non-exit node.");
    _num_outer_preds[idx]++;
  }

public:
  WpoNode(WpoNode&&) = default;
  WpoNode(const WpoNode&) = delete;
  WpoNode& operator=(const WpoNode&) = delete;
  WpoNode& operator=(WpoNode&&) = delete;

  template <typename T1, typename T2>
  friend class wpo_impl::WpoBuilder;
}; // end class WpoNode

/// \brief Weak Partial Ordering.
template <typename GraphRef, typename GraphTrait = GraphTraits<GraphRef>>
class Wpo final {
private:
  using WpoNodeT = WpoNode<GraphRef, GraphTrait>;
  using NodeRef = typename GraphTrait::NodeRef;
  using Type = typename WpoNodeT::Type;
  using WpoIdx = uint32_t;

private:
  // Wpo nodes.
  std::vector<WpoNodeT> _nodes;
  // Top level nodes. Nodes that are outside of any component.
  std::vector<WpoIdx> _toplevel;
  // Predecessors of outer edges in the CFG.
  tbb::concurrent_unordered_map<NodeRef, uint32_t> _post_dfn;
  // No 'sneaky' edges.
  bool _lifted;

  uint32_t get_post_dfn(NodeRef n) {
    auto it = _post_dfn.find(n);
    if (it != _post_dfn.end()) {
      return it->second;
    }
    return 0;
  }

public:
  /// \brief Compute the weak partial order of the given CFG.
  Wpo(const GraphRef& cfg, bool lift)
    : _lifted(lift) {
    auto root = GraphTrait::entry(cfg);
    if (GraphTrait::successor_begin(root) == GraphTrait::successor_end(root)) {
      _nodes.emplace_back(root, Type::Plain, /*size=*/1);
      _toplevel.push_back(0);
      return;
    }
    wpo_impl::WpoBuilder<GraphRef, GraphTrait> builder(cfg,
                                                       _nodes,
                                                       _toplevel,
                                                       _post_dfn,
                                                       lift);
  }

  /// \brief Total number of nodes in this wpo.
  uint32_t size() const {
    return _nodes.size();
  }

  /// \brief Entry node of this wpo.
  WpoIdx get_entry() {
    return _nodes.size() - 1;
  } 

  /// \brief Successors of the node.
  const std::set<WpoIdx>& get_successors(WpoIdx idx) const {
    return _nodes[idx].get_successors();
  }
  
  /// \brief Predecessors of the node.
  const std::set<WpoIdx>& get_predecessors(WpoIdx idx) const {
    return _nodes[idx].get_predecessors();
  }
  
  /// \brief Number of predecessors of the node.
  uint32_t get_num_preds(WpoIdx idx) const {
    return _nodes[idx].get_num_preds();
  }

  /// \brief Get number of outer preds for the exit's component.
  const std::unordered_map<WpoIdx, uint32_t>& get_num_outer_preds(WpoIdx exit) const {
    return _nodes[exit].get_num_outer_preds();
  }

  /// \brief Head of the exit node.
  WpoIdx get_head_of_exit(WpoIdx exit) const {
    return exit + 1;
  }

  /// \brief Exit of the head node.
  WpoIdx get_exit_of_head(WpoIdx head) const {
    return head - 1;
  }

  /// \brief NodeRef for the node.
  const NodeRef& get_node(WpoIdx idx) const {
    return _nodes[idx].get_node();
  }
  
  /// \brief Type queries for node.
  bool is_plain(WpoIdx idx) const {
    return _nodes[idx].is_plain();
  }
  bool is_head(WpoIdx idx) const {
    return _nodes[idx].is_head();
  }
  bool is_exit(WpoIdx idx) const {
    return _nodes[idx].is_exit();
  }

  /// \brief Check whether a predecessor is outside of the component.
  bool is_from_outside(NodeRef head, NodeRef pred) {
    return get_post_dfn(head) < get_post_dfn(pred);
  }

  Wpo(const Wpo& other) = delete;
  Wpo(Wpo&& other) = delete;
  Wpo& operator=(const Wpo& other) = delete;
  Wpo& operator=(Wpo&& other) = delete;
}; // end class Wpo

namespace wpo_impl {
template <typename GraphRef, typename GraphTrait = GraphTraits<GraphRef>>
class WpoBuilder final {
 private:
  using NodeRef = typename GraphTrait::NodeRef;
  using WpoNodeT = WpoNode<GraphRef, GraphTrait>;
  using WpoT = Wpo<GraphRef, GraphTrait>;
  using Type = typename WpoNodeT::Type;
  using WpoIdx = uint32_t;

 public:
  WpoBuilder(const GraphRef& cfg,
             std::vector<WpoNodeT>& wpo_space,
             std::vector<WpoIdx>& toplevel,
             tbb::concurrent_unordered_map<NodeRef, uint32_t>& post_dfn,
             bool lift)
      : _wpo_space(wpo_space),
        _toplevel(toplevel),
        _post_dfn(post_dfn),
        _next_dfn(1), _next_post_dfn(1), _next_idx(0), _lift(lift) {
    construct_auxilary(cfg);
    construct_wpo();
    // Compute num_outer_preds.
    for (auto& p : _for_outer_preds) {
      auto& v = p.first;
      auto& x_max = p.second;
      auto h = _wpo_space[v].is_head() ? v : _parent[v];
      // index of exit == index of head - 1.
      auto x = h - 1;
      while (x != x_max) {
        _wpo_space[x].inc_num_outer_preds(v);
        h = _parent[h];
        x = h - 1;
      }
      _wpo_space[x].inc_num_outer_preds(v);
    }
  }

 private:
  /// \brief Construct auxilary data-structures.
  /// Performs DFS iteratively to classify the edges
  /// and to find lowest common ancestors of cross/forward edges.
  /// Nodes are identified by their DFNs in the main construction algorithm.
  void construct_auxilary(const GraphRef& cfg) {
    typedef std::unordered_map<uint32_t,std::size_t> rank_t;
    typedef std::unordered_map<uint32_t,uint32_t> parent_t;
    rank_t rank_map;
    parent_t parent_map;
    typedef boost::associative_property_map<rank_t> r_pmap_t;
    r_pmap_t r_pmap(rank_map);
    typedef boost::associative_property_map<parent_t> p_pmap_t;
    p_pmap_t p_pmap(parent_map);
    boost::disjoint_sets<r_pmap_t,p_pmap_t> dsets(r_pmap, p_pmap);
    std::unordered_map<uint32_t, uint32_t> ancestor;

    std::stack<std::tuple<NodeRef,bool,uint32_t>> stack;
    std::unordered_map<uint32_t, bool> black;

    stack.push(std::make_tuple(GraphTrait::entry(cfg), false, 0));
    while (!stack.empty()) {
      // Iterative DFS.
      auto& stack_top = stack.top();
      auto vertex_ref = std::get<0>(stack_top);
      auto finished = std::get<1>(stack_top);
      auto pred = std::get<2>(stack_top);
      stack.pop();

      if (finished) {
        // DFS is done with this vertex.
        // Post DFN.
        _post_dfn[vertex_ref] = _next_post_dfn++;

        auto vertex = get_dfn(vertex_ref);
        black[vertex] = true;

        dsets.union_set(vertex, pred);
        ancestor[dsets.find_set(pred)] = pred;
      } else {
        if (get_dfn(vertex_ref) != 0 /* means vertex is already discovered. */) {
          if (pred != 0) {
            // A forward edge.
            // Forward edges can be ignored, as they are redundant.
          }
          continue;
        }
        // New vertex is discovered.
        auto vertex = _next_dfn++;
        push_ref(vertex_ref);
        set_dfn(vertex_ref, vertex);
        dsets.make_set(vertex);
        ancestor[vertex] = vertex;

        // This will be popped after all its successors are finished.
        stack.push(std::make_tuple(vertex_ref, true, pred));

        auto successors = std::vector<NodeRef>(GraphTrait::successor_begin(vertex_ref),
                                               GraphTrait::successor_end(vertex_ref));
        for (auto rit = successors.rbegin(); rit != successors.rend(); ++rit) {
          auto succ = get_dfn(*rit);
          if (succ == 0 /* 0 means vertex is undiscovered. */) {
            // Newly discovered vertex. Search continues.
            stack.push(std::make_tuple(*rit, false, vertex));
          } else if (black[succ]) {
            // A cross edge.
            auto lca = ancestor[dsets.find_set(succ)];
            _cross_fwds[lca].emplace_back(vertex, succ);
          } else {
            // A back edge.
            _back_preds[succ].push_back(vertex);
          }
        }
        if (pred != 0) {
          // A tree edge.
          _non_back_preds[vertex].push_back(pred);
        }
      }
    }
  }

  void construct_wpo() {
    std::vector<uint32_t> rank(get_next_dfn());
    std::vector<uint32_t> parent(get_next_dfn());
    // Partition of vertices. Each subset is known to be strongly connected.
    boost::disjoint_sets<uint32_t*, uint32_t*> dsets(&rank[0], &parent[0]);
    // Maps representative of a set to the vertex with minimum DFN.
    std::vector<uint32_t> rep(get_next_dfn());
    // Maps a head to its exit.
    std::vector<uint32_t> exit(get_next_dfn());
    // Maps a vertex to original non-back edges that now target the vertex.
    std::vector<std::vector<std::pair<uint32_t, uint32_t>>> origin(get_next_dfn());
    // Maps a head to its size of components.
    std::vector<uint32_t> size(get_next_dfn());
    // Index of WpoNode in wpo space.
    _d2i.resize(2*get_next_dfn());
    // DFN that will be assigned to the next exit.
    uint32_t dfn = get_next_dfn();

    // Initialization.
    for (uint32_t v = 1; v < get_next_dfn(); v++) {
      dsets.make_set(v);
      rep[v] = exit[v] = v;
      for (auto u : _non_back_preds[v]) {
        origin[v].emplace_back(u, v);
      }
    }
    // In reverse DFN, build WPOs for SCCs bottom-up.
    for (uint32_t h = get_next_dfn()-1; h > 0; h--) {
      // Restore cross/fwd edges which has h as the LCA.
      auto it = _cross_fwds.find(h);
      if (it != _cross_fwds.end()) {
        for (auto& edge : it->second) {
          // edge: u -> v
          auto& u = edge.first;
          auto& v = edge.second;
          auto rep_v = rep[dsets.find_set(v)];
          _non_back_preds[rep_v].push_back(u);
          origin[rep_v].emplace_back(u, v);
        }
      }

      // Find nested SCCs.
      bool is_SCC = false;
      std::unordered_set<uint32_t> backpreds_h;
      for (auto v : _back_preds[h]) {
        if (v != h) {
          backpreds_h.insert(rep[dsets.find_set(v)]);
        } else {
          // Self-loop.
          is_SCC = true;
        }
      }
      if (!backpreds_h.empty()) {
        is_SCC = true;
      }
      // Invariant: h \notin backpreds_h.
      std::unordered_set<uint32_t> nested_SCCs_h(backpreds_h);
      std::vector<uint32_t> worklist_h(backpreds_h.begin(), backpreds_h.end());
      while (!worklist_h.empty()) {
        auto v = worklist_h.back();
        worklist_h.pop_back();
        for (auto p : _non_back_preds[v]) {
          auto rep_p = rep[dsets.find_set(p)];
          auto it = nested_SCCs_h.find(rep_p);
          if (it == nested_SCCs_h.end() && rep_p != h) {
            nested_SCCs_h.insert(rep_p);
            worklist_h.push_back(rep_p);
          }
        }
      }
      // Invariant: h \notin nested_SCCs_h.

      // h is a Trivial SCC.
      if (!is_SCC) {
        size[h] = 1;
        add_node(h, get_ref(h), Type::Plain, /*size=*/1);
        // Invariant: wpo_space = ...::h
        continue;
      }

      // Compute the size of the component C_h.
      // Size of this component is initialized to 2: the head and exit.
      uint32_t size_h = 2;
      for (auto v : nested_SCCs_h) {
        size_h += size[v];
      }
      size[h] = size_h;
      // Invariant: size_h = size[h] = number of nodes in the component C_h.

      // Add new exit x_h.
      auto x_h = dfn++;
      add_node(x_h, get_ref(h), Type::Exit, size_h);
      add_node(h, get_ref(h), Type::Head, size_h);
      // Invariant: wpo_space = ...::x_h::h
      if (backpreds_h.empty()) {
        // Add scheduling constraints from h to x_h
        add_successor(/*from=*/h, /*to=*/x_h, /*exit=*/x_h, /*outer_pred?=*/false);
      } else {
        for (auto p : backpreds_h) {
          add_successor(/*from=*/exit[p], /*to=*/x_h, /*exit=*/x_h, /*outer_pred?=*/false);
        }
      }
      // Invariant: Scheduling contraints to x_h are all constructed.

      // Add scheduling constraints between the WPOs for nested SCCs.
      for (auto v : nested_SCCs_h) {
        for (auto& edge : origin[v]) {
          auto& u = edge.first;
          auto& vv = edge.second;
          auto& x_u = exit[rep[dsets.find_set(u)]];
          auto& x_v = exit[v];
          // Invariant: u -> vv, u \notin C_v, vv \in C_v, u \in C_h, v \in C_h.
          if (_lift) {
            add_successor(/*from=*/x_u, /*to=*/v, /*exit=*/x_v, /*outer_pred?=*/x_v != v);
            // Invariant: x_u \in get_predecessors(v).
          } else {
            add_successor(/*from=*/x_u, /*to=*/vv, /*exit=*/x_v, /*outer_pred?=*/x_v != v);
            // Invariant: x_u \in get_predecessors(vv).
          }
        }
      }
      // Invariant: WPO for SCC with h as its header is constructed.

      // Update the partition by merging.
      for (auto v : nested_SCCs_h) {
        dsets.union_set(v, h);
        rep[dsets.find_set(v)] = h;
        _parent[index_of(v)] = index_of(h);
      }

      // Set exit of h to x_h.
      exit[h] = x_h;
      // Invariant: exit[h] = h if C_h is trivial SCC, x_h otherwise.
    }

    // Add scheduling constraints between the WPOs for maximal SCCs.
    for (uint32_t v = 1; v < get_next_dfn(); v++) {
      if (rep[dsets.find_set(v)] == v) {
        add_toplevel(v);
        _parent[index_of(v)] = index_of(v);

        for (auto& edge : origin[v]) {
          auto& u = edge.first;
          auto& vv = edge.second;
          auto& x_u = exit[rep[dsets.find_set(u)]];
          auto& x_v = exit[v];
          // Invariant: u -> vv, u \notin C_v, vv \in C_v, u \in C_h, v \in C_h.
          if (_lift) {
            add_successor(/*from=*/x_u, /*to=*/v, /*exit=*/x_v, /*outer_pred?=*/x_v != v);
            // Invariant: x_u \in get_predecessors(v).
          } else {
            add_successor(/*from=*/x_u, /*to=*/vv, /*exit=*/x_v, /*outer_pred?=*/x_v != v);
            // Invariant: x_u \in get_predecessors(vv).
          }
        }
      }
    }
    // Invariant: WPO for the CFG is constructed.
  }

  uint32_t get_dfn(NodeRef n) {
    auto it = _dfn.find(n);
    if (it != _dfn.end()) {
      return it->second;
    }
    return 0;
  }

  void set_dfn(NodeRef n, uint32_t num) {
    _dfn[n] = num;
  }

  const NodeRef& get_ref(uint32_t num) const {
    return _ref.at(num-1);
  }

  void push_ref(NodeRef n) {
    _ref.push_back(n);
  }

  uint32_t get_next_dfn() const {
    // Includes exits.
    // 0 represents invalid node.
    return _next_dfn;
  }

  void add_node(uint32_t dfn, NodeRef ref, Type type, uint32_t size) {
    _d2i[dfn] = _next_idx++;
    _wpo_space.emplace_back(ref, type, size);
  }

  WpoNodeT& node_of(uint32_t dfn) {
    return _wpo_space[index_of(dfn)];
  }

  WpoIdx index_of(uint32_t dfn) {
    return _d2i[dfn];
  }

  void add_successor(uint32_t from, uint32_t to, uint32_t exit, bool outer_pred) {
    auto fromIdx = index_of(from);
    auto toIdx = index_of(to);
    auto& fromNode = node_of(from);
    auto& toNode = node_of(to);
    if (!fromNode.is_successor(toIdx)) {
      if (outer_pred) {
        _for_outer_preds.push_back(std::make_pair(toIdx, index_of(exit)));
      }
      fromNode.add_successor(toIdx);
      toNode.add_predecessor(fromIdx);
    }
  }

  void add_toplevel(uint32_t what) {
    _toplevel.push_back(index_of(what));
  }

  /// \brief A reference to Wpo space (array of Wpo nodes).
  std::vector<WpoNodeT>& _wpo_space;
  /// \brief A reference to Wpo space that contains only the top level nodes.
  std::vector<WpoIdx>& _toplevel;
  /// \brief A map from GraphRef to DFN.
  std::unordered_map<NodeRef, uint32_t> _dfn;
  /// \brief A map from GraphRef to post DFN.
  tbb::concurrent_unordered_map<NodeRef, uint32_t>& _post_dfn;
  /// \brief A map from DFN to GraphRef.
  std::vector<NodeRef> _ref;
  /// \brief A map from DFN to DFNs of its backedge predecessors.
  std::unordered_map<uint32_t, std::vector<uint32_t>> _back_preds;
  /// \brief A map from DFN to DFNs of its non-backedge predecessors.
  std::unordered_map<uint32_t, std::vector<uint32_t>> _non_back_preds;
  /// \brief A map from DFN to cross/forward edges (DFN is the lowest common ancestor).
  std::unordered_map<uint32_t, std::vector<std::pair<uint32_t, uint32_t>>> _cross_fwds;
  /// \brief Increase m_num_outer_preds[x][pair.first] for component C_x that satisfies
  // pair.first \in C_x \subseteq C_{pair.second}.
  std::vector<std::pair<WpoIdx, WpoIdx>> _for_outer_preds;
  /// \brief A map from node to the head of minimal component that contains it as
  // non-header.
  std::unordered_map<WpoIdx, WpoIdx> _parent;
  /// \brief Next DFN to assign.
  uint32_t _next_dfn;
  /// \brief Next post DFN to assign.
  uint32_t _next_post_dfn;
  /// \brief Next WpoIdx to assign.
  uint32_t _next_idx;
  /// \brief Map DFN to WpoIdx.
  std::vector<uint32_t> _d2i;
  /// \brief Lift the scheduling constraint when adding.
  bool _lift;
}; // end class wpo_builder

} // end namespace wpo_impl

} // end namespace core
} // end namespace ikos
