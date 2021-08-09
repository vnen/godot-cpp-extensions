#ifndef GODOT_OBJECT_HPP
#define GODOT_OBJECT_HPP

#include "defs.hpp"
#include "variant/variant.hpp"

#include <classes/object.hpp>

#include <godot-headers/gdnative_interface.h>

#define ADD_PROPERTY(m_property, m_setter, m_getter) ClassDB::add_property(get_class_static(), m_property, m_setter, m_getter)

namespace godot {

struct PropertyInfo {
	Variant::Type type = Variant::NIL;
	const char *name = nullptr;
	const char *class_name = nullptr;
	uint32_t hint = 0;
	const char *hint_string = nullptr;
	uint32_t usage = 7;

	operator GDNativePropertyInfo() {
		GDNativePropertyInfo info;
		info.type = type;
		info.name = name;
		info.hint = hint;
		info.hint_string = hint_string;
		info.class_name = class_name;
		info.usage = usage;
		return info;
	}

	PropertyInfo() = default;

	PropertyInfo(Variant::Type p_type, const char *p_name, PropertyHint p_hint = PROPERTY_HINT_NONE, const char *p_hint_string = "", uint32_t p_usage = PROPERTY_USAGE_DEFAULT, const char *p_class_name = "") :
			type(p_type),
			name(p_name),
			hint(p_hint),
			hint_string(p_hint_string),
			usage(p_usage) {
		if (hint == PROPERTY_HINT_RESOURCE_TYPE) {
			class_name = hint_string;
		} else {
			class_name = p_class_name;
		}
	}

	PropertyInfo(GDNativeVariantType p_type, const char *p_name, PropertyHint p_hint = PROPERTY_HINT_NONE, const char *p_hint_string = "", uint32_t p_usage = PROPERTY_USAGE_DEFAULT, const char *p_class_name = "") :
			PropertyInfo((Variant::Type)p_type, p_name, p_hint, p_hint_string, p_usage, p_class_name) {}
};

// typedef void GodotObject;

// class Object {
// protected:
// 	GodotObject *_owner = nullptr;

// 	static void (*_get_bind_methods())() {
// 		return &Object::_bind_methods;
// 	}

// public:
// 	static void initialize_class();
// 	static void _bind_methods();
// };

template <class T>
T *Object::cast_to(Object *p_object) {
	// FIXME
	return (T *)p_object;
}

} // namespace godot

#endif // ! GODOT_OBJECT_HPP
