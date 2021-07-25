#ifndef GODOT_CPP_BUILTIN_PTRCALL_HPP
#define GODOT_CPP_BUILTIN_PTRCALL_HPP

#include <godot-headers/gdnative_interface.h>

#include <array>

namespace godot {

namespace internal {

template <class... Args>
void _call_builtin_constructor(const GDNativePtrConstructor constructor, GDNativeTypePtr base, Args... args) {
	std::array<const GDNativeTypePtr, sizeof...(Args)> call_args = { (const GDNativeTypePtr)args... };
	constructor(base, call_args.data());
}

template <class T, class... Args>
T _call_builtin_method_ptr_ret(const GDNativePtrBuiltInMethod method, GDNativeTypePtr base, Args... args) {
	T ret;
	std::array<const GDNativeTypePtr, sizeof...(Args)> call_args = { (const GDNativeTypePtr)args... };
	method(base, call_args.data(), &ret, sizeof...(Args));
	return ret;
}

template <class... Args>
void _call_builtin_method_ptr_no_ret(const GDNativePtrBuiltInMethod method, GDNativeTypePtr base, Args... args) {
	std::array<const GDNativeTypePtr, sizeof...(Args)> call_args = { (const GDNativeTypePtr)args... };
	method(base, call_args.data(), nullptr, sizeof...(Args));
}

template <class T>
T _call_builtin_operator_ptr(const GDNativePtrOperatorEvaluator op, const GDNativeTypePtr left, const GDNativeTypePtr right) {
	T ret;
	op(left, right, &ret);
	return ret;
}

template <class T>
T _call_builtin_ptr_getter(const GDNativePtrGetter getter, const GDNativeTypePtr base) {
	T ret;
	getter(base, &ret);
	return ret;
}

} // namespace internal

} // namespace godot

#endif // ! GODOT_CPP_BUILTIN_PTRCALL_HPP
