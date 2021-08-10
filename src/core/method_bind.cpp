#include "method_bind.hpp"

namespace godot {

const char *MethodBind::get_name() const {
	return name;
}

void MethodBind::set_name(const char *p_name) {
	name = p_name;
}

void MethodBind::set_argument_count(int p_count) {
	argument_count = p_count;
}

void MethodBind::set_const(bool p_const) {
	_is_const = p_const;
}

void MethodBind::set_return(bool p_return) {
	_has_return = p_return;
}

void MethodBind::set_argument_names(const std::vector<std::string> &p_names) {
	argument_names = p_names;
}

std::vector<std::string> MethodBind::get_argument_names() const {
	return argument_names;
}

void MethodBind::generate_argument_types(int p_count) {
	set_argument_count(p_count);

	argument_types = new GDNativeVariantType[p_count + 1];

	// -1 means return type.
	for (int i = -1; i < p_count; i++) {
		argument_types[i + 1] = gen_argument_type(i);
	}
}

GDNativePropertyInfo MethodBind::get_argument_info(int p_argument) const {
	GDNativePropertyInfo info = gen_argument_type_info(p_argument);
	if (p_argument >= 0) {
		info.name = p_argument < (int)argument_names.size() ? argument_names[p_argument].c_str() : "";
	}
	return info;
}

GDNativeVariantType MethodBind::bind_get_argument_type(void *p_method_userdata, int32_t p_argument) {
	const MethodBind *bind = reinterpret_cast<const MethodBind *>(p_method_userdata);
	return bind->get_argument_type(p_argument);
}

void MethodBind::bind_get_argument_info(void *p_method_userdata, int32_t p_argument, GDNativePropertyInfo *r_info) {
	const MethodBind *bind = reinterpret_cast<const MethodBind *>(p_method_userdata);
	*r_info = bind->get_argument_info(p_argument);
}

GDNativeExtensionClassMethodArgumentMetadata MethodBind::bind_get_argument_metadata(void *p_method_userdata, int32_t p_argument) {
	const MethodBind *bind = reinterpret_cast<const MethodBind *>(p_method_userdata);
	return bind->get_argument_metadata(p_argument);
}

void MethodBind::bind_call(void *p_method_userdata, GDExtensionClassInstancePtr p_instance, const GDNativeVariantPtr *p_args, const GDNativeInt p_argument_count, GDNativeVariantPtr r_return, GDNativeCallError *r_error) {
	const MethodBind *bind = reinterpret_cast<const MethodBind *>(p_method_userdata);
	Variant ret = bind->call(p_instance, p_args, p_argument_count, *r_error);
	// This assumes the return value is an empty Variant, so it doesn't need to call the destructor first.
	// Since only NativeExtensionMethodBind calls this from the Godot side, it should always be the case.
	internal::interface->variant_new_copy(r_return, ret.ptr);
}

void MethodBind::bind_ptrcall(void *p_method_userdata, GDExtensionClassInstancePtr p_instance, const GDNativeTypePtr *p_args, GDNativeTypePtr r_return) {
	const MethodBind *bind = reinterpret_cast<const MethodBind *>(p_method_userdata);
	bind->ptrcall(p_instance, p_args, r_return);
}

MethodBind::~MethodBind() {
	if (argument_types) {
		delete[] argument_types;
	}
}

} // namespace godot
