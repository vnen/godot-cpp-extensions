#ifndef GODOT_CPP_ENGINE_PTRCALL_HPP
#define GODOT_CPP_ENGINE_PTRCALL_HPP

#include <godot-headers/gdnative_interface.h>

#include <core/binder_common.hpp>
#include <core/object.hpp>
#include <godot.hpp>

#include <array>

namespace godot {

namespace internal {

template <class O, class... Args>
Object *_call_native_mb_ret_obj(const GDNativeMethodBindPtr mb, void *instance, Args... args) {
	GodotObject *ret = nullptr;
	std::array<const GDNativeTypePtr, sizeof...(Args)> mb_args = { (const GDNativeTypePtr)args... };
	internal::interface->object_method_bind_ptrcall(mb, instance, mb_args.data(), &ret);
	return (Object *)internal::interface->object_get_instance_binding(ret, internal::token, &O::___binding_callbacks);
}

template <class R, class... Args>
R _call_native_mb_ret(const GDNativeMethodBindPtr mb, void *instance, Args... args) {
	R ret;
	std::array<const GDNativeTypePtr, sizeof...(Args)> mb_args = { (const GDNativeTypePtr)args... };
	internal::interface->object_method_bind_ptrcall(mb, instance, mb_args.data(), &ret);
	return ret;
}

template <class... Args>
void _call_native_mb_no_ret(GDNativeMethodBindPtr mb, void *instance, Args... args) {
	std::array<const GDNativeTypePtr, sizeof...(Args)> mb_args = { (const GDNativeTypePtr)args... };
	internal::interface->object_method_bind_ptrcall(mb, instance, mb_args.data(), nullptr);
}

template <class R, class... Args>
R _call_utility_ret(GDNativePtrUtilityFunction func, Args... args) {
	R ret;
	std::array<const GDNativeTypePtr, sizeof...(Args)> mb_args = { (const GDNativeTypePtr)args... };
	func(&ret, mb_args.data(), mb_args.size());
	return ret;
}

template <class... Args>
Object *_call_utility_ret_obj(const GDNativePtrUtilityFunction func, void *instance, Args... args) {
	GodotObject *ret = nullptr;
	std::array<const GDNativeTypePtr, sizeof...(Args)> mb_args = { (const GDNativeTypePtr)args... };
	func(&ret, mb_args.data(), mb_args.size());
	return (Object *)internal::interface->object_get_instance_binding(ret, internal::token, &Object::___binding_callbacks);
}

template <class... Args>
void _call_utility_no_ret(GDNativePtrUtilityFunction func, Args... args) {
	std::array<const GDNativeTypePtr, sizeof...(Args)> mb_args = { (const GDNativeTypePtr)args... };
	func(nullptr, mb_args.data(), mb_args.size());
}

} // namespace internal

} // namespace godot

#endif // ! GODOT_CPP_ENGINE_PTRCALL_HPP
