#ifndef GODOT_OBJECT_HPP
#define GODOT_OBJECT_HPP

#include "defs.hpp"

#include <godot-headers/gdnative_interface.h>

#define GDCLASS(m_class, m_inherits)                                           \
private:                                                                       \
	friend class ClassDB;                                                      \
                                                                               \
protected:                                                                     \
	static void (*_get_bind_methods())() {                                     \
		return &m_class::_bind_methods;                                        \
	}                                                                          \
                                                                               \
public:                                                                        \
	static void initialize_class() {                                           \
		static bool initialized = false;                                       \
		if (initialized) {                                                     \
			return;                                                            \
		}                                                                      \
		m_inherits::initialize_class();                                        \
		if (m_class::_get_bind_methods() != m_inherits::_get_bind_methods()) { \
			_bind_methods();                                                   \
		}                                                                      \
		initialized = true;                                                    \
	}                                                                          \
                                                                               \
	static const char *get_class_static() {                                    \
		return #m_class;                                                       \
	}                                                                          \
                                                                               \
	static const char *get_parent_class_static() {                             \
		return #m_inherits;                                                    \
	}                                                                          \
                                                                               \
	static GDExtensionClassInstancePtr create(void *data) {                    \
		return memnew(m_class);                                                \
	}                                                                          \
                                                                               \
	static void free(void *data, GDExtensionClassInstancePtr ptr) {            \
		memdelete(reinterpret_cast<m_class *>(ptr));                           \
	}                                                                          \
                                                                               \
private:

namespace godot {

class Object {
protected:
	static void (*_get_bind_methods())() {
		return &Object::_bind_methods;
	}

public:
    static void initialize_class();
    static void _bind_methods();
};

}

#endif // ! GODOT_OBJECT_HPP
