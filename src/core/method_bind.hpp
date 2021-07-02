#ifndef GODOT_CPP_METHOD_BIND_HPP
#define GODOT_CPP_METHOD_BIND_HPP

#include "binder_common.hpp"
#include "type_info.hpp"

#include <godot-headers/gdnative_interface.h>

#include <string>
#include <vector>

#include <iostream>

namespace godot {

enum MethodFlags {
	METHOD_FLAG_NORMAL = 1,
	METHOD_FLAG_EDITOR = 2,
	METHOD_FLAG_NOSCRIPT = 4,
	METHOD_FLAG_CONST = 8,
	METHOD_FLAG_REVERSE = 16, // used for events
	METHOD_FLAG_VIRTUAL = 32,
	METHOD_FLAG_FROM_SCRIPT = 64,
	METHOD_FLAG_VARARG = 128,
	METHOD_FLAG_STATIC = 256,
	METHOD_FLAGS_DEFAULT = METHOD_FLAG_NORMAL,
};

class MethodBind {
    const char *name = nullptr;
    const char *instance_class = nullptr;
    size_t argument_count = 0;
    uint32_t hint_flags = METHOD_FLAG_NORMAL;

    bool _is_const = false;
    bool _has_return = false;

    std::vector<std::string> argument_names;
    GDNativeVariantType *argument_types = nullptr;

protected:
    virtual GDNativeVariantType gen_argument_type(int p_arg) const = 0;
	virtual GDNativePropertyInfo gen_argument_type_info(int p_arg) const = 0;
	void generate_argument_types(int p_count);
    void set_const(bool p_const);
    void set_return(bool p_return);
    void set_argument_count(size_t p_count);

public:
    const char *get_name() const;
    void set_name(const char *p_name);
    size_t get_argument_count() const;
    const char *get_instance_class() const;
    void set_instance_class(const char* p_class);
    bool is_const() const;
    bool has_return() const;

    void set_argument_names(const std::vector<std::string> &p_names);
    std::vector<std::string> get_argument_names() const;

    _FORCE_INLINE_ GDNativeVariantType get_argument_type(int p_argument) const {
        // TODO: // ERR_FAIL_COND_V(p_argument < -1 || p_argument > argument_count, Variant::NIL);
		return argument_types[p_argument + 1];
    }

    GDNativePropertyInfo get_argument_info(int p_argument) const;
    virtual GDNativeExtensionClassMethodArgumentMetadata get_argument_metadata(int p_argument) const = 0;

    virtual void call(GDExtensionClassInstancePtr p_instance, const GDNativeVariantPtr *p_args, const GDNativeInt p_argument_count, GDNativeVariantPtr r_return, GDNativeCallError *r_error) const = 0;
	virtual void ptrcall(GDExtensionClassInstancePtr p_instance, const GDNativeTypePtr *p_args, GDNativeTypePtr r_return) const = 0;

    // Extension info.
    static GDNativeVariantType bind_get_argument_type(void *p_method_userdata, int32_t p_argument);
    static void bind_get_argument_info(void *p_method_userdata, int32_t p_argument, GDNativePropertyInfo *r_info);
    static GDNativeExtensionClassMethodArgumentMetadata bind_get_argument_metadata(void *p_method_userdata, int32_t p_argument);

    static void bind_call(void *p_method_userdata, GDExtensionClassInstancePtr p_instance, const GDNativeVariantPtr *p_args, const GDNativeInt p_argument_count, GDNativeVariantPtr r_return, GDNativeCallError *r_error);
	static void bind_ptrcall(void *p_method_userdata, GDExtensionClassInstancePtr p_instance, const GDNativeTypePtr *p_args, GDNativeTypePtr r_return);

    virtual ~MethodBind();
};

template <class T, class... P>
class MethodBindT : public MethodBind {
	void (T::*method)(P...);

protected:

// GCC raises warnings in the case P = {} as the comparison is always false...
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wlogical-op"
#endif
	virtual GDNativeVariantType gen_argument_type(int p_arg) const {
		if (p_arg >= 0 && p_arg < (int)sizeof...(P)) {
			return call_get_argument_type<P...>(p_arg);
		} else {
			return GDNATIVE_VARIANT_TYPE_NIL;
		}
	}

	virtual GDNativePropertyInfo gen_argument_type_info(int p_arg) const {
		GDNativePropertyInfo pi;
		if (p_arg >= 0 && p_arg < (int)sizeof...(P)) {
            call_get_argument_type_info<P...>(p_arg, pi);
        } else {
            pi = PropertyInfo();
        }
        return pi;
	}
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif


public:
    virtual GDNativeExtensionClassMethodArgumentMetadata get_argument_metadata(int p_argument) const {
        return call_get_argument_metadata<P...>(p_argument);
    }

    virtual void call(GDExtensionClassInstancePtr p_instance, const GDNativeVariantPtr *p_args, const GDNativeInt p_argument_count, GDNativeVariantPtr r_return, GDNativeCallError *r_error) const {
        // TODO
        std::cout << "Function " << get_name() << " is being called with variant." << std::endl;
        r_error->error = NATIVE_CALL_ERROR_INVALID_METHOD;
        // T *self = reinterpret_cast<T *>(p_instance);
        // (self->*(method))();

    }
	virtual void ptrcall(GDExtensionClassInstancePtr p_instance, const GDNativeTypePtr *p_args, GDNativeTypePtr r_ret) const {
        // TODO
        std::cout << "Function " << get_name() << " is being called with ptr." << std::endl;
        call_with_ptr_args<T, P...>(reinterpret_cast<T *>(p_instance), method, p_args);
    }

    MethodBindT(void (T::*p_method)(P...)) {
		method = p_method;
		set_argument_count(sizeof...(P));
        generate_argument_types(sizeof...(P));
	}
};

template <class T, class R, class... P>
class MethodBindTR : public MethodBind {
	R(T::*method)(P...);

protected:
// GCC raises warnings in the case P = {} as the comparison is always false...
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wlogical-op"
#endif
	virtual GDNativeVariantType gen_argument_type(int p_arg) const {
		if (p_arg >= 0 && p_arg < (int)sizeof...(P)) {
			return call_get_argument_type<P...>(p_arg);
		} else {
			return GetTypeInfo<R>::VARIANT_TYPE;
		}
	}

	virtual GDNativePropertyInfo gen_argument_type_info(int p_arg) const {
		if (p_arg >= 0 && p_arg < (int)sizeof...(P)) {
			GDNativePropertyInfo pi;
			call_get_argument_type_info<P...>(p_arg, pi);
			return pi;
		} else {
			return GetTypeInfo<R>::get_class_info();
		}
	}
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

public:
    virtual GDNativeExtensionClassMethodArgumentMetadata get_argument_metadata(int p_argument) const {
		if (p_argument >= 0) {
			return call_get_argument_metadata<P...>(p_argument);
		} else {
			return GetTypeInfo<R>::METADATA;
		}
	}

    virtual void call(GDExtensionClassInstancePtr p_instance, const GDNativeVariantPtr *p_args, const GDNativeInt p_argument_count, GDNativeVariantPtr r_return, GDNativeCallError *r_error) const {
        // TODO
        std::cout << "Function " << get_name() << " is being called with variant." << std::endl;
        r_error->error = NATIVE_CALL_ERROR_INVALID_METHOD;
        // T *self = reinterpret_cast<T *>(p_instance);
        // (self->*(method))();
    }
	virtual void ptrcall(GDExtensionClassInstancePtr p_instance, const GDNativeTypePtr *p_args, GDNativeTypePtr r_ret) const {
        // TODO
        std::cout << "Function " << get_name() << " is being called with ptr." << std::endl;
        call_with_ptr_args_ret<T, R, P...>(reinterpret_cast<T *>(p_instance), method, p_args, r_ret);
    }

    MethodBindTR(R (T::*p_method)(P...)) {
		method = p_method;
        generate_argument_types(sizeof...(P));
		set_argument_count(sizeof...(P));
        set_return(true);
	}
};

template <class T, class... P>
MethodBind *create_method_bind(void (T::*p_method)(P...)) {
    MethodBind *a = memnew((MethodBindT<T, P...>)(p_method));
    a->set_instance_class(T::get_class_static());
    return a;
}

template <class T, class R, class... P>
MethodBind *create_method_bind(R (T::*p_method)(P...)) {
    MethodBind *a = memnew((MethodBindTR<T, R, P...>)(p_method));
    a->set_instance_class(T::get_class_static());
    return a;
}

}

#endif // ! GODOT_CPP_METHOD_BIND_HPP
