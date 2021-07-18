#ifndef GODOT_CPP_BUILTIN_PTRCALL_HPP
#define GODOT_CPP_BUILTIN_PTRCALL_HPP

#include <godot-headers/gdnative_interface.h>

#include <array>

namespace godot {

namespace internal {

template <class... Args>
void _call_builtin_constructor(GDNativePtrConstructor constructor, GDNativeTypePtr base, Args... args) {
	std::array<void *const, sizeof...(Args)> call_args = { (void *const)args... };
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

template <class T>
T _call_builtin_operator_ptr(GDNativePtrOperatorEvaluator op, GDNativeTypePtr left, GDNativeTypePtr right) {
	T ret;
	op(left, right, &ret);
	return ret;
}

template <class T>
T _call_builtin_ptr_getter(GDNativePtrGetter getter, GDNativeTypePtr base) {
	T ret;
	getter(base, &ret);
	return ret;
}

} // namespace internal

} // namespace godot

#endif // ! GODOT_CPP_BUILTIN_PTRCALL_HPP
