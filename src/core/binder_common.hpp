#ifndef GODOT_CPP_BINDER_COMMON_HPP
#define GODOT_CPP_BINDER_COMMON_HPP

#include <godot-headers/gdnative_interface.h>

#include "method_ptrcall.hpp"
#include "type_info.hpp"

namespace godot {

template <class T, class... P, size_t... Is>
void call_with_ptr_args_helper(T *p_instance, void (T::*p_method)(P...), const GDNativeTypePtr *p_args, IndexSequence<Is...>) {
	(p_instance->*p_method)(PtrToArg<P>::convert(p_args[Is])...);
}

template <class T, class... P, size_t... Is>
void call_with_ptr_argsc_helper(T *p_instance, void (T::*p_method)(P...) const, const GDNativeTypePtr *p_args, IndexSequence<Is...>) {
	(p_instance->*p_method)(PtrToArg<P>::convert(p_args[Is])...);
}

template <class T, class R, class... P, size_t... Is>
void call_with_ptr_args_ret_helper(T *p_instance, R (T::*p_method)(P...), const GDNativeTypePtr *p_args, void *r_ret, IndexSequence<Is...>) {
	PtrToArg<R>::encode((p_instance->*p_method)(PtrToArg<P>::convert(p_args[Is])...), r_ret);
}

template <class T, class R, class... P, size_t... Is>
void call_with_ptr_args_retc_helper(T *p_instance, R (T::*p_method)(P...) const, const GDNativeTypePtr *p_args, void *r_ret, IndexSequence<Is...>) {
	PtrToArg<R>::encode((p_instance->*p_method)(PtrToArg<P>::convert(p_args[Is])...), r_ret);
}

template <class T, class... P>
void call_with_ptr_args(T *p_instance, void (T::*p_method)(P...), const GDNativeTypePtr *p_args) {
	call_with_ptr_args_helper<T, P...>(p_instance, p_method, p_args, BuildIndexSequence<sizeof...(P)>{});
}

template <class T, class... P>
void call_with_ptr_argsc(T *p_instance, void (T::*p_method)(P...) const, const GDNativeTypePtr *p_args) {
	call_with_ptr_argsc_helper<T, P...>(p_instance, p_method, p_args, BuildIndexSequence<sizeof...(P)>{});
}

template <class T, class R, class... P>
void call_with_ptr_args_ret(T *p_instance, R (T::*p_method)(P...), const GDNativeTypePtr *p_args, void *r_ret) {
	call_with_ptr_args_ret_helper<T, R, P...>(p_instance, p_method, p_args, r_ret, BuildIndexSequence<sizeof...(P)>{});
}

template <class T, class R, class... P>
void call_with_ptr_args_retc(T *p_instance, R (T::*p_method)(P...) const, const GDNativeTypePtr *p_args, void *r_ret) {
	call_with_ptr_args_retc_helper<T, R, P...>(p_instance, p_method, p_args, r_ret, BuildIndexSequence<sizeof...(P)>{});
}

// GCC raises "parameter 'p_args' set but not used" when P = {},
// it's not clever enough to treat other P values as making this branch valid.
#if defined(DEBUG_METHODS_ENABLED) && defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-parameter"
#endif

template <class Q>
void call_get_argument_type_helper(int p_arg, int &index, GDNativeVariantType &type) {
	if (p_arg == index) {
		type = GetTypeInfo<Q>::VARIANT_TYPE;
	}
	index++;
}

template <class... P>
GDNativeVariantType call_get_argument_type(int p_arg) {
	GDNativeVariantType type = GDNATIVE_VARIANT_TYPE_NIL;
	int index = 0;
	// I think rocket science is simpler than modern C++.
	using expand_type = int[];
	expand_type a{ 0, (call_get_argument_type_helper<P>(p_arg, index, type), 0)... };
	(void)a; // Suppress (valid, but unavoidable) -Wunused-variable warning.
	(void)index; // Suppress GCC warning.
	return type;
}

template <class Q>
void call_get_argument_type_info_helper(int p_arg, int &index, GDNativePropertyInfo &info) {
	if (p_arg == index) {
		info = GetTypeInfo<Q>::get_class_info();
	}
	index++;
}

template <class... P>
void call_get_argument_type_info(int p_arg, GDNativePropertyInfo &info) {
	int index = 0;
	// I think rocket science is simpler than modern C++.
	using expand_type = int[];
	expand_type a{ 0, (call_get_argument_type_info_helper<P>(p_arg, index, info), 0)... };
	(void)a; // Suppress (valid, but unavoidable) -Wunused-variable warning.
	(void)index; // Suppress GCC warning.
}

template <class Q>
void call_get_argument_metadata_helper(int p_arg, int &index, GDNativeExtensionClassMethodArgumentMetadata &md) {
	if (p_arg == index) {
		md = GetTypeInfo<Q>::METADATA;
	}
	index++;
}

template <class... P>
GDNativeExtensionClassMethodArgumentMetadata call_get_argument_metadata(int p_arg) {
	GDNativeExtensionClassMethodArgumentMetadata md = GDNATIVE_EXTENSION_METHOD_ARGUMENT_METADATA_NONE;

	int index = 0;
	// I think rocket science is simpler than modern C++.
	using expand_type = int[];
	expand_type a{ 0, (call_get_argument_metadata_helper<P>(p_arg, index, md), 0)... };
	(void)a; // Suppress (valid, but unavoidable) -Wunused-variable warning.
	(void)index;
	return md;
}

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

}

#endif // ! GODOT_CPP_BINDER_COMMON_HPP
