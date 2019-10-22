/*******************************************************************************
 *
 * \file
 * \brief Implementation of MemoryLocation
 *
 * Author: Clement Decoodt
 *
 * Contributors: Maxime Arthaud
 *
 * Contact: ikos@lists.nasa.gov
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

#include <ikos/analyzer/analysis/memory_location.hpp>
#include <ikos/analyzer/util/source_location.hpp>

#include <mutex>

std::mutex mtx_mem_local;
std::mutex mtx_mem_global;
std::mutex mtx_mem_function;
std::mutex mtx_mem_aggregate;
std::mutex mtx_mem_dyn_alloc;

namespace ikos {
namespace analyzer {

// MemoryLocation

MemoryLocation::MemoryLocation(MemoryLocationKind kind) : _kind(kind) {}

MemoryLocation::~MemoryLocation() = default;

// LocalMemoryLocation

LocalMemoryLocation::LocalMemoryLocation(ar::LocalVariable* var)
    : MemoryLocation(LocalMemoryKind), _var(var) {
  ikos_assert(this->_var != nullptr);
}

void LocalMemoryLocation::dump(std::ostream& o) const {
  this->_var->dump(o);
}

// GlobalMemoryLocation

GlobalMemoryLocation::GlobalMemoryLocation(ar::GlobalVariable* var)
    : MemoryLocation(GlobalMemoryKind), _var(var) {
  ikos_assert(this->_var != nullptr);
}

void GlobalMemoryLocation::dump(std::ostream& o) const {
  this->_var->dump(o);
}

// FunctionMemoryLocation

FunctionMemoryLocation::FunctionMemoryLocation(ar::Function* fun)
    : MemoryLocation(FunctionMemoryKind), _fun(fun) {
  ikos_assert(this->_fun != nullptr);
}

void FunctionMemoryLocation::dump(std::ostream& o) const {
  o << "@" << this->_fun->name();
}

// AggregateMemoryLocation

AggregateMemoryLocation::AggregateMemoryLocation(ar::InternalVariable* var)
    : MemoryLocation(AggregateMemoryKind), _var(var) {
  ikos_assert(this->_var != nullptr);
}

void AggregateMemoryLocation::dump(std::ostream& o) const {
  this->_var->dump(o);
}

// AbsoluteZeroMemoryLocation

AbsoluteZeroMemoryLocation::AbsoluteZeroMemoryLocation()
    : MemoryLocation(AbsoluteZeroMemoryKind) {}

void AbsoluteZeroMemoryLocation::dump(std::ostream& o) const {
  o << "absolute_zero";
}

// ArgvMemoryLocation

ArgvMemoryLocation::ArgvMemoryLocation() : MemoryLocation(ArgvMemoryKind) {}

void ArgvMemoryLocation::dump(std::ostream& o) const {
  o << "argv";
}

// LibcErrnoMemoryLocation

LibcErrnoMemoryLocation::LibcErrnoMemoryLocation()
    : MemoryLocation(LibcErrnoMemoryKind) {}

void LibcErrnoMemoryLocation::dump(std::ostream& o) const {
  o << "libc.errno";
}

// DynAllocMemoryLocation

DynAllocMemoryLocation::DynAllocMemoryLocation(ar::CallBase* call,
                                               CallContext* context)
    : MemoryLocation(DynAllocMemoryKind), _call(call), _context(context) {
  ikos_assert(this->_call != nullptr && this->_context != nullptr);
}

void DynAllocMemoryLocation::dump(std::ostream& o) const {
  ikos_assert_msg(this->_call->code()->is_function_body(),
                  "dynamic allocation in global variable initializer");
  auto fun = this->_call->code()->function();
  o << "dyn_alloc:" << fun->name() << ":";

  SourceLocation loc = source_location(this->_call);
  if (loc) {
    o << loc.line() << ":" << loc.column();
  } else {
    o << this->_call;
  }

  if (!this->_context->empty()) {
    o << ":" << this->_context;
  }
}

// MemoryFactory

MemoryFactory::MemoryFactory()
    : _absolute_zero(std::make_unique< AbsoluteZeroMemoryLocation >()),
      _argv(std::make_unique< ArgvMemoryLocation >()),
      _libc_errno(std::make_unique< LibcErrnoMemoryLocation >()) {}

MemoryFactory::~MemoryFactory() = default;

LocalMemoryLocation* MemoryFactory::get_local(ar::LocalVariable* var) {
  std::lock_guard<std::mutex> lck (mtx_mem_local);
  auto [it, result] = this->_local_memory_map.try_emplace(var,
      lazy_convert_construct([&] {
        auto ml = new LocalMemoryLocation(var);
        return std::unique_ptr< LocalMemoryLocation >(ml);
      }));
  return it->second.get();
  /*
  auto it = this->_local_memory_map.find(var);
  if (it == this->_local_memory_map.end()) {
    auto ml = new LocalMemoryLocation(var);
    this->_local_memory_map.try_emplace(var,
                                        std::unique_ptr< LocalMemoryLocation >(
                                            ml));
    return ml;
  } else {
    return it->second.get();
  }
  */
}

GlobalMemoryLocation* MemoryFactory::get_global(ar::GlobalVariable* var) {
  std::lock_guard<std::mutex> lck (mtx_mem_global);
  auto [it, result] = this->_global_memory_map.try_emplace(var,
      lazy_convert_construct([&] {
        auto ml = new GlobalMemoryLocation(var);
        return std::unique_ptr< GlobalMemoryLocation >(ml);
      }));
  return it->second.get();
  /*
  auto it = this->_global_memory_map.find(var);
  if (it == this->_global_memory_map.end()) {
    auto ml = new GlobalMemoryLocation(var);
    this->_global_memory_map
        .try_emplace(var, std::unique_ptr< GlobalMemoryLocation >(ml));
    return ml;
  } else {
    return it->second.get();
  }
  */
}

FunctionMemoryLocation* MemoryFactory::get_function(ar::Function* fun) {
  std::lock_guard<std::mutex> lck (mtx_mem_function);
  auto [it, result] = this->_function_memory_map.try_emplace(fun,
      lazy_convert_construct([&] {
        auto ml = new FunctionMemoryLocation(fun);
        return std::unique_ptr< FunctionMemoryLocation >(ml);
      }));
  return it->second.get();
  /*
  auto it = this->_function_memory_map.find(fun);
  if (it == this->_function_memory_map.end()) {
    auto ml = new FunctionMemoryLocation(fun);
    this->_function_memory_map
        .try_emplace(fun, std::unique_ptr< FunctionMemoryLocation >(ml));
    return ml;
  } else {
    return it->second.get();
  }
  */
}

FunctionMemoryLocation* MemoryFactory::get_function(
    ar::FunctionPointerConstant* cst) {
  return this->get_function(cst->function());
}

AggregateMemoryLocation* MemoryFactory::get_aggregate(
    ar::InternalVariable* var) {
  std::lock_guard<std::mutex> lck (mtx_mem_aggregate);
  auto [it, result] = this->_aggregate_memory_map.try_emplace(var,
      lazy_convert_construct([&] {
        auto ml = new AggregateMemoryLocation(var);
        return std::unique_ptr< AggregateMemoryLocation >(ml);
      }));
  return it->second.get();
  /*
  auto it = this->_aggregate_memory_map.find(var);
  if (it == this->_aggregate_memory_map.end()) {
    auto ml = new AggregateMemoryLocation(var);
    this->_aggregate_memory_map
        .try_emplace(var, std::unique_ptr< AggregateMemoryLocation >(ml));
    return ml;
  } else {
    return it->second.get();
  }
  */
}

AbsoluteZeroMemoryLocation* MemoryFactory::get_absolute_zero() {
  return this->_absolute_zero.get();
}

ArgvMemoryLocation* MemoryFactory::get_argv() {
  return this->_argv.get();
}

LibcErrnoMemoryLocation* MemoryFactory::get_libc_errno() {
  return this->_libc_errno.get();
}

DynAllocMemoryLocation* MemoryFactory::get_dyn_alloc(ar::CallBase* call,
                                                     CallContext* context) {
  std::lock_guard<std::mutex> lck (mtx_mem_dyn_alloc);
  auto [it, result] = this->_dyn_alloc_map.try_emplace({call, context},
      lazy_convert_construct([&] {
        auto ml = new DynAllocMemoryLocation(call, context);
        return std::unique_ptr< DynAllocMemoryLocation >(ml);
      }));
  return it->second.get();
  /*
  auto it = this->_dyn_alloc_map.find({call, context});
  if (it == this->_dyn_alloc_map.end()) {
    auto ml = new DynAllocMemoryLocation(call, context);
    this->_dyn_alloc_map.try_emplace({call, context},
                                     std::unique_ptr< DynAllocMemoryLocation >(
                                         ml));
    return ml;
  } else {
    return it->second.get();
  }
  */
}

} // end namespace analyzer
} // end namespace ikos
