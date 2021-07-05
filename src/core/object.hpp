#ifndef GODOT_OBJECT_HPP
#define GODOT_OBJECT_HPP

#include "defs.hpp"
#include "variant.hpp"

#include <godot-headers/gdnative_interface.h>

#define GDCLASS(m_class, m_inherits)                                                                               \
private:                                                                                                           \
	friend class ClassDB;                                                                                          \
                                                                                                                   \
protected:                                                                                                         \
	static void (*_get_bind_methods())() {                                                                         \
		return &m_class::_bind_methods;                                                                            \
	}                                                                                                              \
                                                                                                                   \
public:                                                                                                            \
	static void initialize_class() {                                                                               \
		static bool initialized = false;                                                                           \
		if (initialized) {                                                                                         \
			return;                                                                                                \
		}                                                                                                          \
		m_inherits::initialize_class();                                                                            \
		if (m_class::_get_bind_methods() != m_inherits::_get_bind_methods()) {                                     \
			_bind_methods();                                                                                       \
		}                                                                                                          \
		initialized = true;                                                                                        \
	}                                                                                                              \
                                                                                                                   \
	static const char *get_class_static() {                                                                        \
		return #m_class;                                                                                           \
	}                                                                                                              \
                                                                                                                   \
	static const char *get_parent_class_static() {                                                                 \
		return #m_inherits;                                                                                        \
	}                                                                                                              \
                                                                                                                   \
	static GDExtensionClassInstancePtr create(void *data) {                                                        \
		return memnew(m_class);                                                                                    \
	}                                                                                                              \
                                                                                                                   \
	static void free(void *data, GDExtensionClassInstancePtr ptr) {                                                \
		memdelete(reinterpret_cast<m_class *>(ptr));                                                               \
	}                                                                                                              \
                                                                                                                   \
	static void set_object_instance(GDExtensionClassInstancePtr p_instance, GDNativeObjectPtr p_object_instance) { \
		reinterpret_cast<m_class *>(p_instance)->_owner = (godot::GodotObject *)p_object_instance;                 \
	}                                                                                                              \
                                                                                                                   \
private:

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

	PropertyInfo(GDNativeVariantType p_type, const char *p_name) {
		type = (Variant::Type)p_type;
		name = p_name;
	}

	PropertyInfo(Variant::Type p_type, const char *p_name) {
		type = p_type;
		name = p_name;
	}
};

typedef void GodotObject;

class Object {
protected:
	GodotObject *_owner = nullptr;

	static void (*_get_bind_methods())() {
		return &Object::_bind_methods;
	}

public:
	static void initialize_class();
	static void _bind_methods();
};

} // namespace godot

#endif // ! GODOT_OBJECT_HPP
