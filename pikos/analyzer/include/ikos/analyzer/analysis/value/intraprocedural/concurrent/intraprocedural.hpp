#pragma once

#include <ikos/analyzer/analysis/context.hpp>

namespace ikos {
namespace analyzer {
namespace cc {

/// \brief Intraprocedural value analysis
class IntraproceduralValueAnalysis {
private:
  /// \brief Analysis context
  Context& _ctx;

public:
  /// \brief Constructor
  explicit IntraproceduralValueAnalysis(Context& ctx);

  /// \brief Deleted copy constructor
  IntraproceduralValueAnalysis(const IntraproceduralValueAnalysis&) = delete;

  /// \brief Deleted move constructor
  IntraproceduralValueAnalysis(IntraproceduralValueAnalysis&&) = delete;

  /// \brief Deleted copy assignment operator
  IntraproceduralValueAnalysis& operator=(const IntraproceduralValueAnalysis&) =
      delete;

  /// \brief Deleted move assignment operator
  IntraproceduralValueAnalysis& operator=(IntraproceduralValueAnalysis&&) =
      delete;

  /// \brief Destructor
  ~IntraproceduralValueAnalysis();

  /// \brief Run the analysis
  void run();

}; // end class IntraproceduralValueAnalysis

} // end namespace cc
} // end namespace analyzer
} // end namespace ikos
