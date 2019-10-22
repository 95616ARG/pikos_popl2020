#include <memory>
#include <vector>

#include <ikos/analyzer/analysis/value/abstract_domain.hpp>
#include <ikos/analyzer/analysis/value/intraprocedural/concurrent/function_fixpoint.hpp>
#include <ikos/analyzer/analysis/value/intraprocedural/concurrent/intraprocedural.hpp>
#include <ikos/analyzer/checker/checker.hpp>
#include <ikos/analyzer/util/demangle.hpp>
#include <ikos/analyzer/util/log.hpp>
#include <ikos/analyzer/util/progress.hpp>
#include <ikos/analyzer/util/timer.hpp>

namespace ikos {
namespace analyzer {
namespace cc {

IntraproceduralValueAnalysis::IntraproceduralValueAnalysis(Context& ctx)
    : _ctx(ctx) {}

IntraproceduralValueAnalysis::~IntraproceduralValueAnalysis() = default;

void IntraproceduralValueAnalysis::run() {
  using namespace value;
  using namespace value::intraprocedural;

  tbb::task_scheduler_init init(_ctx.opts.numthreads == 0 ? tbb::task_scheduler_init::automatic 
                                                          : _ctx.opts.numthreads);


  // Bundle
  ar::Bundle* bundle = _ctx.bundle;

  // Create checkers
  std::vector< std::unique_ptr< Checker > > checkers;
  for (CheckerName name : _ctx.opts.analyses) {
    checkers.emplace_back(make_checker(_ctx, name));
  }

  // Initial invariant
  AbstractDomain init_inv(
      /*normal=*/value::MemoryAbstractDomain(
          value::PointerAbstractDomain(value::make_top_machine_int_domain(
                                           _ctx.opts.machine_int_domain),
                                       value::NullityAbstractDomain::top()),
          value::UninitializedAbstractDomain::top(),
          value::LifetimeAbstractDomain::top()),
      /*caught_exceptions=*/value::MemoryAbstractDomain::bottom(),
      /*propagated_exceptions=*/value::MemoryAbstractDomain::bottom());

  // Setup a progress logger
  std::unique_ptr< ProgressLogger > progress =
      make_progress_logger(_ctx.opts.progress,
                           LogLevel::Info,
                           /* num_tasks = */
                           2 * std::count_if(bundle->function_begin(),
                                             bundle->function_end(),
                                             [](ar::Function* fun) {
                                               return fun->is_definition();
                                             }));
  ScopeLogger scope(*progress);

  // Analyze every function in the bundle
  for (auto it = bundle->function_begin(), et = bundle->function_end();
       it != et;
       ++it) {
    ar::Function* function = *it;

    // Insert the function in the database
    _ctx.output_db->functions.insert(function);

    if (!function->is_definition()) {
      continue;
    }

    concurrent::FunctionFixpoint fixpoint(_ctx, function);

    {
      progress->start_task("(Concurrently) Analyzing function '" + demangle(function->name()) +
                           "'");
      ScopeTimerDatabase t(_ctx.output_db->times,
                           "ikos-analyzer.value." + function->name());
      fixpoint.run(init_inv);
    }

    if (_ctx.opts.use_checks) {
      progress->start_task("Checking properties for function '" +
                           demangle(function->name()) + "'");
      ScopeTimerDatabase t(_ctx.output_db->times,
                           "ikos-analyzer.check." + function->name());
      fixpoint.run_checks(checkers);
    }
  }
}

} // end namespace cc
} // end namespace analyzer
} // end namespace ikos
