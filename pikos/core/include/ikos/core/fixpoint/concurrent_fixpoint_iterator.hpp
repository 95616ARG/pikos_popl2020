#pragma once

#include <ikos/core/domain/abstract_domain.hpp>
#include <ikos/core/semantic/graph.hpp>

namespace ikos {
namespace core {

/// \brief Base class for forward fixpoint iterators
template < typename GraphRef,
           typename AbstractValue,
           typename GraphTrait = GraphTraits< GraphRef > >
class ConcurrentForwardFixpointIterator {
public:
  static_assert(IsGraph< GraphRef, GraphTrait >::value,
                "GraphRef does not implement GraphTraits");
  static_assert(IsAbstractDomain< AbstractValue >::value,
                "AbstractValue does not implement AbstractDomain");

public:
  /// \brief Reference to a node of the graph
  using NodeRef = typename GraphTrait::NodeRef;

public:
  /// \brief Default constructor
  ConcurrentForwardFixpointIterator() = default;

  /// \brief Copy constructor
  ConcurrentForwardFixpointIterator(const ConcurrentForwardFixpointIterator&) = delete;

  /// \brief Move constructor
  ConcurrentForwardFixpointIterator(ConcurrentForwardFixpointIterator&&) = delete;

  /// \brief Copy assignment operator
  ConcurrentForwardFixpointIterator& operator=(const ConcurrentForwardFixpointIterator&) = delete;

  /// \brief Move assignment operator
  ConcurrentForwardFixpointIterator& operator=(ConcurrentForwardFixpointIterator&&) = delete;

  /// \brief Semantic transformer for a node
  ///
  /// This method is called with the abstract value representing the state
  /// of the program upon entering the node. The method should return an
  /// abstract value representing the state of the program after the node.
  virtual AbstractValue analyze_node(NodeRef node, AbstractValue state) = 0;

  /// \brief Semantic transformer for an edge
  ///
  /// This method is called with the abstract value representing the state of
  /// the program after exiting the source node. The method should return an
  /// abstract value representing the state of the program after the edge, at
  /// the entry of the destination node.
  virtual AbstractValue analyze_edge(NodeRef src,
                                     NodeRef dest,
                                     AbstractValue state) = 0;

  /// \brief Process the computed abstract value for a node
  ///
  /// This method is called when the fixpoint is reached, and with the abstract
  /// value representing the state of the program upon entering the node.
  virtual void process_pre(NodeRef, const AbstractValue&) = 0;

  /// \brief Process the computed abstract value for a node
  ///
  /// This method is called when the fixpoint is reached, and with the abstract
  /// value representing the state of the program after the node.
  virtual void process_post(NodeRef, const AbstractValue&) = 0;

  /// \brief Destructor
  virtual ~ConcurrentForwardFixpointIterator() = default;

}; // end class ConcurrentForwardFixpointIterator

} // end namespace core
} // end namespace ikos
