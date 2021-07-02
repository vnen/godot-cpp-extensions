#ifndef GODOT_CPP_VARIANT_HPP
#define GODOT_CPP_VARIANT_HPP

#include "defs.hpp"

class Variant {
public:
	enum Type {
		NIL,

		// atomic types
		BOOL,
		INT,
		FLOAT,
		STRING,

		// math types
		VECTOR2,
		VECTOR2I,
		RECT2,
		RECT2I,
		VECTOR3,
		VECTOR3I,
		TRANSFORM2D,
		PLANE,
		QUATERNION,
		AABB,
		BASIS,
		TRANSFORM3D,

		// misc types
		COLOR,
		STRING_NAME,
		NODE_PATH,
		RID,
		OBJECT,
		CALLABLE,
		SIGNAL,
		DICTIONARY,
		ARRAY,

		// typed arrays
		PACKED_BYTE_ARRAY,
		PACKED_INT32_ARRAY,
		PACKED_INT64_ARRAY,
		PACKED_FLOAT32_ARRAY,
		PACKED_FLOAT64_ARRAY,
		PACKED_STRING_ARRAY,
		PACKED_VECTOR2_ARRAY,
		PACKED_VECTOR3_ARRAY,
		PACKED_COLOR_ARRAY,

		VARIANT_MAX
	};
};

class String {};
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

#endif // ! GODOT_CPP_VARIANT_HPP
