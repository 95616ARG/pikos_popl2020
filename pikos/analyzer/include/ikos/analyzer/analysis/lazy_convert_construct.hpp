#pragma once

template<class Factory>
struct lazy_convert_construct
{
  using result_type = std::invoke_result_t<const Factory&>; // Use some traits to check what would be the return type of the lambda if called.

  constexpr lazy_convert_construct(Factory&& factory) 
    : factory_(std::move(factory)) // Let's store the factory for a latter usage.
  {
  }

  //                                     ↓ Respect the same nowthrow properties as the lambda factory.
  constexpr operator result_type() const noexcept(std::is_nothrow_invocable_v<const Factory&>) 
  //        ^ enable       ^ the type this struct can be converted to 
  //          conversion
  {
    return factory_();  // Delegate the conversion job to the lambda factory.
  }
  Factory factory_;
};
