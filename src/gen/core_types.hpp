#ifndef GODOT_CORE_TYPES_HPP
#define GODOT_CORE_TYPES_HPP

// FIXME: Temporary. Putting all in on file because it's easier to handle.

#include <cstddef>
#include <cstdint>

#include <godot-headers/gdnative_interface.h>

namespace godot {

class Array {};
class Vector2 {};
class Vector2i {
public:
    union {
		int32_t x = 0;
		int32_t width;
	};
	union {
		int32_t y = 0;
		int32_t height;
	};
};
class Vector3 {};
class Vector3i {};

class String {
	// This size should come from api.json
	static constexpr size_t STRING_SIZE = 8;
	uint8_t opaque[STRING_SIZE] { 0 };

	static GDNativePtrConstructor string_copy_ptr;
	static GDNativePtrDestructor ptr_destructor;

	friend class Variant;

	static void init_bindings();

public:
	String();
	String(const char *p_latin_string);
	String(const String &p_string);

	~String();
};


class NodePath {
	// This size should come from api.json
	static constexpr size_t NODE_PATH_SIZE = 8;
	uint8_t opaque[NODE_PATH_SIZE] { 0 };

	static GDNativePtrConstructor construct_from_nodepath;
	static GDNativePtrConstructor construct_from_string;
	static GDNativePtrDestructor ptr_destructor;

	friend class Variant;

	static void init_bindings();
public:
    NodePath(const char *);
    NodePath(const String &);
	NodePath(const NodePath &p_string);

	~NodePath();
};

}

#endif // ! GODOT_CORE_TYPES_HPP
