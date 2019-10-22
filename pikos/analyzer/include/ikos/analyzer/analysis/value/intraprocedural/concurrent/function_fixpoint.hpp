#pragma once

#include <ikos/ar/semantic/code.hpp>
#include <ikos/ar/semantic/function.hpp>

#include <ikos/core/fixpoint/concurrent_fwd_fixpoint_iterator.hpp>

#include <ikos/analyzer/analysis/call_context.hpp>
#include <ikos/analyzer/analysis/context.hpp>
#include <ikos/analyzer/analysis/fixpoint_profile.hpp>
#include <ikos/analyzer/analysis/value/abstract_domain.hpp>
#include <ikos/analyzer/checker/checker.hpp>

namespace ikos {
namespace analyzer {
namespace value {
namespace intraprocedural {
namespace concurrent {

/// \brief Fixpoint on a function body
class FunctionFixpoint final
    : public core::InterleavedCFwdFixpointIterator<ar::Code*, AbstractDomain> {
private:
  /// \brief Parent class
  using FwdFixpointIterator =
      core::InterleavedCFwdFixpointIterator< ar::Code*, AbstractDomain >;

private:
  /// \brief Analysis context
  Context& _ctx;

  /// \brief Analyzed function
  ar::Function* _function;

  /// \brief Empty call context
  CallContext* _empty_call_context;

  /// \brief Fixpoint profile
  boost::optional< const FixpointProfile& > _profile;

public:
  /// \brief Create a function fixpoint iterator
  FunctionFixpoint(Context& ctx, ar::Function* function);

  /// \brief Compute the fixpoint
  void run(AbstractDomain inv);

  /// \brief Extrapolate the new state after an increasing iteration
  AbstractDomain extrapolate(ar::BasicBlock* head,
                             unsigned iteration,
                             AbstractDomain before,
                             AbstractDomain after) override;

  /// \brief Check if the decreasing iterations fixpoint is reached
  bool is_decreasing_iterations_fixpoint(const AbstractDomain& before,
                                         const AbstractDomain& after) override;

  /// \brief Propagate the invariant through the basic block
  AbstractDomain analyze_node(ar::BasicBlock* bb, AbstractDomain pre) override;

  /// \brief Propagate the invariant through an edge
  AbstractDomain analyze_edge(ar::BasicBlock* src,
                              ar::BasicBlock* dest,
                              AbstractDomain pre) override;

  /// \brief Process the computed abstract value for a node
  void process_pre(ar::BasicBlock* bb, const AbstractDomain& pre) override;

  /// \brief Process the computed abstract value for a node
  void process_post(ar::BasicBlock* bb, const AbstractDomain& post) override;

  /// \brief Run the checks with the previously computed fix-point
  void run_checks(const std::vector< std::unique_ptr< Checker > >& checkers);

}; // end class FunctionFixpoint

} // end namespace concurrent
} // end namespace intraprocedural
} // end namespace value
} // end namespace analyzer
} // end namespace ikos
