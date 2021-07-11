#ifndef GODOT_CPP_BUILTIN_PTRCALL_HPP
#define GODOT_CPP_BUILTIN_PTRCALL_HPP

#include <godot-headers/gdnative_interface.h>

#include <array>

namespace godot {

namespace internal {

template <class... Args>
void _call_builtin_constructor(GDNativePtrConstructor constructor, GDNativeTypePtr base, Args... args) {
	std::array<void *const, sizeof...(Args)> call_args = { &args... };
	constructor(base, call_args.data());
}

template <class T, class... Args>
T _call_builtin_method_ptr_ret(GDNativePtrBuiltInMethod method, GDNativeTypePtr base, Args... args) {
	T ret;
	std::array<void *const, sizeof...(Args)> call_args = { &args... };
	method(base, call_args.data(), &ret, sizeof...(Args));
	return ret;
}

template <class... Args>
void _call_builtin_method_ptr_no_ret(GDNativePtrBuiltInMethod method, GDNativeTypePtr base, Args... args) {
	std::array<void *const, sizeof...(Args)> call_args = { &args... };
	method(base, call_args.data(), nullptr, sizeof...(Args));
}

} // namespace internal

} // namespace godot

#endif // ! GODOT_CPP_BUILTIN_PTRCALL_HPP
