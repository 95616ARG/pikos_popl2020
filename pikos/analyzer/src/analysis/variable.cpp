/*******************************************************************************
 *
 * \file
 * \brief Variable implementation
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

#include <ikos/analyzer/analysis/variable.hpp>
#include <ikos/analyzer/exception.hpp>

#include <mutex>

std::mutex mtx_var_local;
std::mutex mtx_var_global;
std::mutex mtx_var_internal;
std::mutex mtx_var_asm_ptr;
std::mutex mtx_var_function_ptr;
std::mutex mtx_var_cell;
std::mutex mtx_var_alloc_size;
std::mutex mtx_var_return;
std::mutex mtx_var_named_shadow;

namespace ikos {
namespace analyzer {

// Variable

Variable::Variable(VariableKind kind, ar::Type* type)
    : _kind(kind), _type(type), _offset_var(nullptr) {
  ikos_assert(this->_type != nullptr);
}

Variable::~Variable() = default;

// LocalVariable

LocalVariable::LocalVariable(ar::LocalVariable* var)
    : Variable(LocalVariableKind, var->type()), _var(var) {
  ikos_assert(this->_var != nullptr);
}

void LocalVariable::dump(std::ostream& o) const {
  this->_var->dump(o);
}

// GlobalVariable

GlobalVariable::GlobalVariable(ar::GlobalVariable* var)
    : Variable(GlobalVariableKind, var->type()), _var(var) {
  ikos_assert(this->_var != nullptr);
}

void GlobalVariable::dump(std::ostream& o) const {
  this->_var->dump(o);
}

// InternalVariable

InternalVariable::InternalVariable(ar::InternalVariable* var)
    : Variable(InternalVariableKind, var->type()), _var(var) {
  ikos_assert(this->_var != nullptr);
}

void InternalVariable::dump(std::ostream& o) const {
  return this->_var->dump(o);
}

// InlineAssemblyPointerVariable

InlineAssemblyPointerVariable::InlineAssemblyPointerVariable(
    ar::InlineAssemblyConstant* inline_asm)
    : Variable(InlineAssemblyPointerVariableKind, inline_asm->type()),
      _inline_asm(inline_asm) {
  ikos_assert(this->_inline_asm != nullptr);
}

void InlineAssemblyPointerVariable::dump(std::ostream& o) const {
  o << "%asm:" << this->_inline_asm;
}

// FunctionPointerVariable

FunctionPointerVariable::FunctionPointerVariable(ar::Function* fun)
    : Variable(FunctionPointerVariableKind,
               ar::PointerType::get(fun->context(), fun->type())),
      _fun(fun) {
  ikos_assert(this->_fun != nullptr);
}

void FunctionPointerVariable::dump(std::ostream& o) const {
  o << "@" << this->_fun->name();
}

// CellVariable

CellVariable::CellVariable(ar::Type* type,
                           MemoryLocation* address,
                           MachineInt offset,
                           MachineInt size)
    : Variable(CellVariableKind, type),
      _address(address),
      _offset(std::move(offset)),
      _size(std::move(size)) {
  ikos_assert(this->_address != nullptr);
}

void CellVariable::dump(std::ostream& o) const {
  o << "C{";
  this->_address->dump(o);
  o << "," << this->_offset << "," << this->_size << "}";
}

// OffsetVariable

OffsetVariable::OffsetVariable(ar::Type* type, Variable* pointer)
    : Variable(OffsetVariableKind, type), _pointer(pointer) {
  ikos_assert(this->_pointer != nullptr);
}

void OffsetVariable::dump(std::ostream& o) const {
  this->_pointer->dump(o);
  o << ".offset";
}

// AllocSizeVariable

AllocSizeVariable::AllocSizeVariable(ar::Type* type, MemoryLocation* address)
    : Variable(AllocSizeVariableKind, type), _address(address) {
  ikos_assert(this->_address != nullptr);
}

void AllocSizeVariable::dump(std::ostream& o) const {
  this->_address->dump(o);
  o << ".size";
}

// ReturnVariable

ReturnVariable::ReturnVariable(ar::Function* fun)
    : Variable(ReturnVariableKind, fun->type()->return_type()), _fun(fun) {
  ikos_assert(this->_fun != nullptr);
}

void ReturnVariable::dump(std::ostream& o) const {
  o << this->_fun->name() << ".return";
}

// NamedShadowVariable

NamedShadowVariable::NamedShadowVariable(ar::Type* type, std::string name)
    : Variable(NamedShadowVariableKind, type), _name(std::move(name)) {
  ikos_assert(!this->_name.empty());
}

void NamedShadowVariable::dump(std::ostream& o) const {
  o << this->_name;
}

// UnnamedShadowVariable

UnnamedShadowVariable::UnnamedShadowVariable(ar::Type* type, std::size_t id)
    : Variable(UnnamedShadowVariableKind, type), _id(id) {}

void UnnamedShadowVariable::dump(std::ostream& o) const {
  o << "unnamed." << this->_id;
}

// VariableFactory

VariableFactory::VariableFactory(ar::Bundle* bundle)
    : _ar_context(bundle->context()),
      _size_type(ar::IntegerType::size_type(bundle)) {}

VariableFactory::~VariableFactory() = default;

LocalVariable* VariableFactory::get_local(ar::LocalVariable* var) {
  std::lock_guard<std::mutex> lck (mtx_var_local);
  auto [it, result] = this->_local_variable_map.try_emplace(var,
      lazy_convert_construct([&] {
        auto vn = new LocalVariable(var);
        vn->set_offset_var(
            std::make_unique< OffsetVariable >(this->_size_type, vn));
        return std::unique_ptr< LocalVariable >(vn);
      }));
  return it->second.get();
  /*
  auto it = this->_local_variable_map.find(var);
  if (it == this->_local_variable_map.end()) {
    auto vn = new LocalVariable(var);
    vn->set_offset_var(
        std::make_unique< OffsetVariable >(this->_size_type, vn));
    this->_local_variable_map.try_emplace(var,
                                          std::unique_ptr< LocalVariable >(vn));
    return vn;
  } else {
    return it->second.get();
  }
  */
}

GlobalVariable* VariableFactory::get_global(ar::GlobalVariable* var) {
  std::lock_guard<std::mutex> lck (mtx_var_global);
  auto [it, result] = this->_global_variable_map.try_emplace(var,
      lazy_convert_construct([&] {
        auto vn = new GlobalVariable(var);
        vn->set_offset_var(
            std::make_unique< OffsetVariable >(this->_size_type, vn));
        return std::unique_ptr< GlobalVariable >(vn);
      }));
  return it->second.get();
  /*
  auto it = this->_global_variable_map.find(var);
  if (it == this->_global_variable_map.end()) {
    auto vn = new GlobalVariable(var);
    vn->set_offset_var(
        std::make_unique< OffsetVariable >(this->_size_type, vn));
    this->_global_variable_map.try_emplace(var,
                                           std::unique_ptr< GlobalVariable >(
                                               vn));
    return vn;
  } else {
    return it->second.get();
  }
  */
}

InternalVariable* VariableFactory::get_internal(ar::InternalVariable* var) {
  std::lock_guard<std::mutex> lck (mtx_var_internal);
  auto [it, result] = this->_internal_variable_map.try_emplace(var,
      lazy_convert_construct([&] {
        auto vn = new InternalVariable(var);
        if (vn->type()->is_pointer() || vn->type()->is_aggregate()) {
          vn->set_offset_var(
              std::make_unique< OffsetVariable >(this->_size_type, vn));
        }
        return std::unique_ptr< InternalVariable >(vn);
      }));
  return it->second.get();
  /*
  auto it = this->_internal_variable_map.find(var);
  if (it == this->_internal_variable_map.end()) {
    auto vn = new InternalVariable(var);
    if (vn->type()->is_pointer() || vn->type()->is_aggregate()) {
      vn->set_offset_var(
          std::make_unique< OffsetVariable >(this->_size_type, vn));
    }
    this->_internal_variable_map
        .try_emplace(var, std::unique_ptr< InternalVariable >(vn));
    return vn;
  } else {
    return it->second.get();
  }
  */
}

InlineAssemblyPointerVariable* VariableFactory::get_asm_ptr(
    ar::InlineAssemblyConstant* cst) {
  std::lock_guard<std::mutex> lck (mtx_var_asm_ptr);
  auto [it, result] = this->_inline_asm_pointer_map.try_emplace(cst,
      lazy_convert_construct([&] {
        auto vn = new InlineAssemblyPointerVariable(cst);
        vn->set_offset_var(
            std::make_unique< OffsetVariable >(this->_size_type, vn));
        return std::unique_ptr< InlineAssemblyPointerVariable >(vn);
      }));
  return it->second.get();
  /*
  auto it = this->_inline_asm_pointer_map.find(cst);
  if (it == this->_inline_asm_pointer_map.end()) {
    auto vn = new InlineAssemblyPointerVariable(cst);
    vn->set_offset_var(
        std::make_unique< OffsetVariable >(this->_size_type, vn));
    this->_inline_asm_pointer_map
        .try_emplace(cst, std::unique_ptr< InlineAssemblyPointerVariable >(vn));
    return vn;
  } else {
    return it->second.get();
  }
  */
}

FunctionPointerVariable* VariableFactory::get_function_ptr(ar::Function* fun) {
  std::lock_guard<std::mutex> lck (mtx_var_function_ptr);
  auto [it, result] = this->_function_pointer_map.try_emplace(fun,
      lazy_convert_construct([&] {
        auto vn = new FunctionPointerVariable(fun);
        vn->set_offset_var(
            std::make_unique< OffsetVariable >(this->_size_type, vn));
        return std::unique_ptr< FunctionPointerVariable >(vn);
      }));
  return it->second.get();
  /*
  auto it = this->_function_pointer_map.find(fun);
  if (it == this->_function_pointer_map.end()) {
    auto vn = new FunctionPointerVariable(fun);
    vn->set_offset_var(
        std::make_unique< OffsetVariable >(this->_size_type, vn));
    this->_function_pointer_map
        .try_emplace(fun, std::unique_ptr< FunctionPointerVariable >(vn));
    return vn;
  } else {
    return it->second.get();
  }
  */
}

FunctionPointerVariable* VariableFactory::get_function_ptr(
    ar::FunctionPointerConstant* cst) {
  return this->get_function_ptr(cst->function());
}

CellVariable* VariableFactory::get_cell(MemoryLocation* address,
                                        const MachineInt& offset,
                                        const MachineInt& size,
                                        Signedness sign) {
  std::lock_guard<std::mutex> lck (mtx_var_cell);
  auto key = std::make_tuple(address, offset, size);
  auto [it, result] = this->_cell_map.try_emplace(key,
      lazy_convert_construct([&] {
        // Create a memory cell variable
        // A cell can be either an integer, a float or a pointer
        // The integer type should have the right bit-width and be signed
        // The parameter `size` is in bytes, compute bit-width = size * 8
        bool overflow;
        MachineInt eight(8, size.bit_width(), Unsigned);
        MachineInt bit_width = mul(size, eight, overflow);
        if (overflow || !bit_width.fits< unsigned >()) {
          throw LogicError("variable factory: cell size too big");
        }
        ar::Type* type = ar::IntegerType::get(this->_ar_context,
                                              bit_width.to< unsigned >(),
                                              Signed);
        auto vn = new CellVariable(type, address, offset, size);
        vn->set_offset_var(
            std::make_unique< OffsetVariable >(this->_size_type, vn));
        return std::unique_ptr< CellVariable >(vn);
      }));
  return it->second.get();
  /*
  auto key = std::make_tuple(address, offset, size);
  auto it = this->_cell_map.find(key);
  if (it == this->_cell_map.end()) {
    // Create a memory cell variable
    // A cell can be either an integer, a float or a pointer
    // The integer type should have the right bit-width and the given signedness
    // The parameter `size` is in bytes, compute bit-width = size * 8
    bool overflow;
    MachineInt eight(8, size.bit_width(), Unsigned);
    MachineInt bit_width = mul(size, eight, overflow);
    if (overflow || !bit_width.fits< unsigned >()) {
      throw LogicError("variable factory: cell size too big");
    }
    ar::Type* type = ar::IntegerType::get(this->_ar_context,
                                          bit_width.to< unsigned >(),
                                          sign);
    auto vn = new CellVariable(type, address, offset, size);
    vn->set_offset_var(
        std::make_unique< OffsetVariable >(this->_size_type, vn));
    this->_cell_map.emplace(key, std::unique_ptr< CellVariable >(vn));
    return vn;
  } else {
    return it->second.get();
  }
  */
}

AllocSizeVariable* VariableFactory::get_alloc_size(MemoryLocation* address) {
  std::lock_guard<std::mutex> lck (mtx_var_alloc_size);
  auto [it, result] = this->_alloc_size_map.try_emplace(address,
      lazy_convert_construct([&] {
        auto vn = new AllocSizeVariable(this->_size_type, address);
        return std::unique_ptr< AllocSizeVariable >(vn);
      }));
  return it->second.get();
  /*
  auto it = this->_alloc_size_map.find(address);
  if (it == this->_alloc_size_map.end()) {
    auto vn = new AllocSizeVariable(this->_size_type, address);
    this->_alloc_size_map.try_emplace(address,
                                      std::unique_ptr< AllocSizeVariable >(vn));
    return vn;
  } else {
    return it->second.get();
  }
  */
}

ReturnVariable* VariableFactory::get_return(ar::Function* fun) {
  std::lock_guard<std::mutex> lck (mtx_var_return);
  auto [it, result] = this->_return_variable_map.try_emplace(fun,
      lazy_convert_construct([&] {
        auto vn = new ReturnVariable(fun);
        if (vn->type()->is_pointer() || vn->type()->is_aggregate()) {
          vn->set_offset_var(
              std::make_unique< OffsetVariable >(this->_size_type, vn));
        }
        return std::unique_ptr< ReturnVariable >(vn);
      }));
  return it->second.get();
  /*
  auto it = this->_return_variable_map.find(fun);
  if (it == this->_return_variable_map.end()) {
    auto vn = new ReturnVariable(fun);
    if (vn->type()->is_pointer() || vn->type()->is_aggregate()) {
      vn->set_offset_var(
          std::make_unique< OffsetVariable >(this->_size_type, vn));
    }
    this->_return_variable_map.try_emplace(fun,
                                           std::unique_ptr< ReturnVariable >(
                                               vn));
    return vn;
  } else {
    return it->second.get();
  }
  */
}

NamedShadowVariable* VariableFactory::get_named_shadow(ar::Type* type,
                                                       llvm::StringRef name) {
  std::lock_guard<std::mutex> lck (mtx_var_named_shadow);
  auto [it, result] = this->_named_shadow_variable_map.try_emplace(name,
      lazy_convert_construct([&] {
        auto vn = new NamedShadowVariable(type, name);
        if (vn->type()->is_pointer() || vn->type()->is_aggregate()) {
          vn->set_offset_var(
              std::make_unique< OffsetVariable >(this->_size_type, vn));
        }
        return std::unique_ptr< NamedShadowVariable >(vn);
      }));
  return it->second.get();
  /*
  auto it = this->_named_shadow_variable_map.find(name);
  if (it == this->_named_shadow_variable_map.end()) {
    auto vn = new NamedShadowVariable(type, name);
    if (vn->type()->is_pointer() || vn->type()->is_aggregate()) {
      vn->set_offset_var(
          std::make_unique< OffsetVariable >(this->_size_type, vn));
    }
    this->_named_shadow_variable_map
        .try_emplace(name, std::unique_ptr< NamedShadowVariable >(vn));
    return vn;
  } else {
    return it->second.get();
  }
  */
}

UnnamedShadowVariable* VariableFactory::create_unnamed_shadow(ar::Type* type) {
  std::size_t id = this->_unnamed_shadow_variable_vec.size();
  auto vn = new UnnamedShadowVariable(type, id);
  if (vn->type()->is_pointer() || vn->type()->is_aggregate()) {
    vn->set_offset_var(
        std::make_unique< OffsetVariable >(this->_size_type, vn));
  }
  this->_unnamed_shadow_variable_vec.emplace_back(
      std::unique_ptr< UnnamedShadowVariable >(vn));
  return vn;
}

} // end namespace analyzer
} // end namespace ikos
