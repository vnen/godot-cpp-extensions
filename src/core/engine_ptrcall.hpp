#ifndef GODOT_CPP_ENGINE_PTRCALL_HPP
#define GODOT_CPP_ENGINE_PTRCALL_HPP

#include <godot-headers/gdnative_interface.h>

#include <core/binder_common.hpp>
#include <core/object.hpp>
#include <godot.hpp>

#include <array>

namespace godot {

namespace internal {

template<class B, class ...Args>
Object *_call_native_mb_ret_obj(const GDNativeMethodBindPtr mb, void *instance, Args... args) {
    GodotObject *ret = nullptr;
    std::array<const GDNativeTypePtr, sizeof...(Args)> mb_args = { (const GDNativeTypePtr)args... };
	internal::interface->object_method_bind_ptrcall(mb, instance, mb_args.data(), &ret);
    return (Object *)internal::interface->object_get_instance_binding(ret, internal::token, &B::___binding_callbacks);
}

template <class T, class... Args>
T _call_native_mb_ret(const GDNativeMethodBindPtr mb, void *instance, Args... args) {
	T ret;
	std::array<const GDNativeTypePtr, sizeof...(Args)> mb_args = { (const GDNativeTypePtr)args... };
	internal::interface->object_method_bind_ptrcall(mb, instance, mb_args.data(), &ret);
	return ret;
}

template <class... Args>
void _call_native_mb_no_ret(GDNativeMethodBindPtr mb, void *instance, Args... args) {
	std::array<const GDNativeTypePtr, sizeof...(Args)> mb_args = { (const GDNativeTypePtr)args... };
	internal::interface->object_method_bind_ptrcall(mb, instance, mb_args.data(), nullptr);
}

} // namespace internal

} // namespace godot

#endif // ! GODOT_CPP_ENGINE_PTRCALL_HPP
