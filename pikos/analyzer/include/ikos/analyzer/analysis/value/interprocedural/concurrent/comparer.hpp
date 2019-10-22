#pragma once

#include <ikos/analyzer/analysis/context.hpp>

namespace ikos {
namespace analyzer {
namespace cc {

/// \brief Interprocedural value analysis
///
/// Performs a top-down analysis with a memory abstract domain.
class InterproceduralValueComparer {
private:
  /// \brief Comparer context
  Context& _ctx;

public:
  /// \brief Constructor
  explicit InterproceduralValueComparer(Context& ctx);

  /// \brief Deleted copy constructor
  InterproceduralValueComparer(const InterproceduralValueComparer&) = delete;

  /// \brief Deleted move constructor
  InterproceduralValueComparer(InterproceduralValueComparer&&) = delete;

  /// \brief Deleted copy assignment operator
  InterproceduralValueComparer& operator=(const InterproceduralValueComparer&) =
      delete;

  /// \brief Deleted move assignment operator
  InterproceduralValueComparer& operator=(InterproceduralValueComparer&&) =
      delete;

  /// \brief Destructor
  ~InterproceduralValueComparer();

  /// \brief Run the analysis
  int run();

}; // end class InterproceduralValueComparer

} // end namespace cc
} // end namespace analyzer
} // end namespace ikos
