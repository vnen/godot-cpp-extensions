#ifndef GODOT_CPP_ENGINE_PTRCALL_HPP
#define GODOT_CPP_ENGINE_PTRCALL_HPP

#include <godot-headers/gdnative_interface.h>

#include <core/object.hpp>
#include <godot.hpp>

#include <array>

namespace godot {

namespace internal {

template<class ...Args>
Object *_call_native_mb_ret_obj(GDNativeMethodBindPtr mb, void *instance, Args... args) {
    GodotObject *ret = nullptr;
    std::array<void *, sizeof...(Args)> mb_args = { &args... };
	internal::interface->object_method_bind_ptrcall(mb, instance, mb_args.data(), &ret);
    // internal::interface->object_get_instance_binding(ret, internal::interface->token,  )
	return nullptr; // FIXME
}

template <class T, class... Args>
T _call_native_mb_ret(GDNativeMethodBindPtr mb, void *instance, Args... args) {
	T ret;
	std::array<void *, sizeof...(Args)> mb_args = { &args... };
	internal::interface->object_method_bind_ptrcall(mb, instance, mb_args.data(), &ret);
	return ret;
}

template <class... Args>
void _call_native_mb_no_ret(GDNativeMethodBindPtr mb, void *instance, Args... args) {
	std::array<void *, sizeof...(Args)> mb_args = { &args... };
	internal::interface->object_method_bind_ptrcall(mb, instance, mb_args.data(), nullptr);
}

} // namespace internal

} // namespace godot

#endif // ! GODOT_CPP_ENGINE_PTRCALL_HPP
