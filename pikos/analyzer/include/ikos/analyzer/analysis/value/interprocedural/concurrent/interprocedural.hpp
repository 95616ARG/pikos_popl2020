#pragma once

#include <ikos/analyzer/analysis/context.hpp>

namespace ikos {
namespace analyzer {
namespace cc {

/// \brief Interprocedural value analysis
///
/// Performs a top-down analysis with a memory abstract domain.
class InterproceduralValueAnalysis {
private:
  /// \brief Analysis context
  Context& _ctx;

public:
  /// \brief Constructor
  explicit InterproceduralValueAnalysis(Context& ctx);

  /// \brief Deleted copy constructor
  InterproceduralValueAnalysis(const InterproceduralValueAnalysis&) = delete;

  /// \brief Deleted move constructor
  InterproceduralValueAnalysis(InterproceduralValueAnalysis&&) = delete;

  /// \brief Deleted copy assignment operator
  InterproceduralValueAnalysis& operator=(const InterproceduralValueAnalysis&) =
      delete;

  /// \brief Deleted move assignment operator
  InterproceduralValueAnalysis& operator=(InterproceduralValueAnalysis&&) =
      delete;

  /// \brief Destructor
  ~InterproceduralValueAnalysis();

  /// \brief Run the analysis
  void run();

}; // end class InterproceduralValueAnalysis

} // end namespace cc
} // end namespace analyzer
} // end namespace ikos
