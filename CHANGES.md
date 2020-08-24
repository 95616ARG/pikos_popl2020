## Changes made to IKOS
This document lists the files changed in IKOS to implement PIKOS.

- `core/include/ikos/core/fixpoint/wpo.hpp` : This implements the weak partial order, used in guiding the parallel fixpoint computation.
- `core/include/ikos/core/fixpoint/concurrent_fwd_fixpoint_iterator.hpp` : This implements the parallel fixpoint iterator.
- `analyzer/include/ikos/analyzer/analysis/value/interprocedural/concurrent/function_fixpoint.hpp` & `analyzer/src/analysis/value/interprocedural/concurrent/function_fixpoint.cpp` & `analyzer/include/ikos/analyzer/analysis/execution_engine/cc_inliner.hpp` : These implement parallel interprocedural analyzer, extending the parallel fixpoint iterator. Interprocedural analysis is achieved by dynamic inlining.
- `analyzer/include/ikos/analyzer/analysis/value/intraprocedural/concurrent/function_fixpoint.hpp` & `analyzer/src/analysis/value/intraprocedural/concurrent/function_fixpoint.cpp` : These implement parallel intraprocedural analyzer, extending the parallel fixpoint iterator.
- `analyzer/include/ikos/analyzer/analysis/fixpoint_profile.hpp` & `analyzer/src/analysis/fixpoint_profile.cpp` : These implement scheduling heuristic based on the partitioning of the basic blocks.
- `analyzer/src/analysis/call_context.cpp` : Added locks on the maps that are concurrently accessed.
- `analyzer/src/analysis/literal.cpp` :  Added locks on the maps that are concurrently accessed.
- `analyzer/src/analysis/memory_location.cpp` :  Added locks on the maps that are concurrently accessed.
- `analyzer/src/analysis/variable.cpp` :  Added locks on the maps that are concurrently accessed.

Miscellaneous
- `core/include/ikos/core/fixpoint/concurrent_fixpoint_iterator.hpp` : Generic API for parallel fixpoint iterators.
- `analyzer/include/ikos/analyzer/analysis/value/interprocedural/concurrent/interprocedural.hpp` & `analyzer/src/analysis/value/interprocedural/concurrent/interprocedural.cpp` : Boilerplates for parallel interprocedural analysis.
- `analyzer/include/ikos/analyzer/analysis/execution_engine/inliner.hpp` : Implemented a way to limit context sentivity in interprocedural analysis.
- `analyzer/include/ikos/analyzer/analysis/value/intraprocedural/concurrent/intraprocedural.hpp` & `analyzer/src/analysis/value/intraprocedural/concurrent/intraprocedural.cpp` : Boilerplates for parallel intraprocedural analysis.
- `analyzer/include/ikos/analyzer/analysis/option.hpp` & `analyzer/src/analysis/option.cpp` : Added options use_concurrent, numthreads, and context_sensitivity.
- `ar/include/ikos/ar/semantic/code.hpp` : Added accessor for the number of the basic blocks.
- `analyzer/python/ikos/analyzer.py` : Added command line arguments.
- `analyzer/src/ikos_analyzer.cpp` : Added concurrent value analysis.
- `analyzer/script/pikos.py.in` : Created separate script for PIKOS.
- `core/include/ikos/core/example/muzq_wpo.hpp` & `core/test/unit/example/muzq_wpo.cpp` & `core/test/unit/CMakeList.txt`: MUZQ test for WPO.
- `analyzer/include/ikos/analyzer/analysis/lazy_convert_construct.hpp` : An attempt to make access to global maps atomic and efficient. (Optional. Requires C++17)
- `analyzer/include/ikos/analyzer/analysis/literal.hpp` : Uses lazy_convert_construct.
- `analyzer/include/ikos/analyzer/analysis/memory_location.hpp` : Uses lazy_convert_construct.
- `analyzer/include/ikos/analyzer/analysis/variable.hpp` : Uses lazy_convert_construct.
- `analyzer/CMakeLists.txt` : Changes in CMake file.
