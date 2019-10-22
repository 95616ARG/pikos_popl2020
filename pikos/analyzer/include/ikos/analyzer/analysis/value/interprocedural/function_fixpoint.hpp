/*******************************************************************************
 *
 * \file
 * \brief Fixpoint on a function body
 *
 * Author: Maxime Arthaud
 *
 * Contact: ikos@lists.nasa.gov
 *
 * Notices:
 *
 * Copyright (c) 2019 United States Government as represented by the
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

#include <ikos/ar/semantic/code.hpp>
#include <ikos/ar/semantic/function.hpp>

#include <ikos/core/fixpoint/fwd_fixpoint_iterator.hpp>

#include <ikos/analyzer/analysis/call_context.hpp>
#include <ikos/analyzer/analysis/context.hpp>
#include <ikos/analyzer/analysis/execution_engine/seq_inliner.hpp>
#include <ikos/analyzer/analysis/execution_engine/numerical.hpp>
#include <ikos/analyzer/analysis/fixpoint_profile.hpp>
#include <ikos/analyzer/analysis/value/abstract_domain.hpp>
#include <ikos/analyzer/analysis/value/interprocedural/progress.hpp>
#include <ikos/analyzer/checker/checker.hpp>

namespace ikos {
namespace analyzer {
namespace value {
namespace interprocedural {

/// \brief Fixpoint on a function body
class FunctionFixpoint final
    : public core::InterleavedFwdFixpointIterator< ar::Code*, AbstractDomain > {
private:
  /// \brief Parent class
  using FwdFixpointIterator =
      core::InterleavedFwdFixpointIterator< ar::Code*, AbstractDomain >;

  /// \brief Numerical execution engine
  using NumericalExecutionEngineT = NumericalExecutionEngine< AbstractDomain >;

  /// \brief Inliner
  using InlineCallExecutionEngineT =
      seq::InlineCallExecutionEngine< FunctionFixpoint, AbstractDomain >;

private:
  /// \brief Analysis context
  Context& _ctx;

  /// \brief Analyzed function
  ar::Function* _function;

  /// \brief Current call context
  CallContext* _call_context;

  /// \brief Fixpoint profile
  boost::optional< const FixpointProfile& > _profile;

  /// \brief Function currently analyzed
  ///
  /// This is used to avoid cycles.
  std::vector< ar::Function* > _analyzed_functions;

  /// \brief List of property checks to run
  const std::vector< std::unique_ptr< Checker > >& _checkers;

  /// \brief Progress logger
  ProgressLogger& _logger;

  /// \brief Numerical execution engine
  NumericalExecutionEngineT _exec_engine;

  /// \brief Call execution engine
  InlineCallExecutionEngineT _call_exec_engine;

public:
  /// \brief Constructor for an entry point
  ///
  /// \param ctx Analysis context
  /// \param checkers List of checkers to run
  /// \param entry_point Function to analyze
  FunctionFixpoint(Context& ctx,
                   const std::vector< std::unique_ptr< Checker > >& checkers,
                   ProgressLogger& logger,
                   ar::Function* entry_point);

  /// \brief Constructor for a callee
  ///
  /// \param ctx Analysis context
  /// \param caller Parent function fixpoint
  /// \param call Call statement
  /// \param callee Called function
  /// \param context_stable Is the calling context stable (fixpoint reached)?
  FunctionFixpoint(Context& ctx,
                   const FunctionFixpoint& caller,
                   ar::CallBase* call,
                   ar::Function* callee,
                   bool context_stable);

  /// \brief Compute the fixpoint
  void run(AbstractDomain inv);

  /// \brief Post invariants are cleared to save memory
  const AbstractDomain& post(ar::BasicBlock*) const = delete;

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

  /// \brief Notify the beginning of the analysis of a cycle
  void notify_enter_cycle(ar::BasicBlock* head) override;

  /// \brief Notify the beginning of an iteration on a cycle
  void notify_cycle_iteration(ar::BasicBlock* head,
                              unsigned iteration,
                              core::FixpointIterationKind kind) override;

  /// \brief Notify the end of the analysis of a cycle
  void notify_leave_cycle(ar::BasicBlock* head) override;

  /// \brief Process the computed abstract value for a node
  void process_pre(ar::BasicBlock* bb, const AbstractDomain& pre) override;

  /// \brief Process the computed abstract value for a node
  void process_post(ar::BasicBlock* bb, const AbstractDomain& post) override;

  /// \brief Run the checks with the previously computed fix-point
  void run_checks();

  /// \name Helpers for InlineCallExecutionEngine
  /// @{

  /// \brief Mark that the calling context is stable
  void mark_context_stable() { this->_call_exec_engine.mark_context_stable(); }

  /// \brief Return true if the given function is currently analyzed
  bool is_currently_analyzed(ar::Function* fun) const;

  bool has_reached_context_sensitivity_limit() const {
    return this->_ctx.opts.context_sensitivity != 0 &&
      this->_analyzed_functions.size() >= this->_ctx.opts.context_sensitivity;
  }

  /// \brief Return the exit invariant, or bottom
  const AbstractDomain& exit_invariant() const {
    return this->_call_exec_engine.exit_invariant();
  }

  /// \brief Return the return statement, or null
  ar::ReturnValue* return_stmt() const {
    return this->_call_exec_engine.return_stmt();
  }

  /// @}

}; // end class FunctionFixpoint

} // end namespace interprocedural
} // end namespace value
} // end namespace analyzer
} // end namespace ikos
