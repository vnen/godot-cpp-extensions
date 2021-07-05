#ifndef GODOT_CPP_VARIANT_HPP
#define GODOT_CPP_VARIANT_HPP

#include "defs.hpp"

#include <gen/core_types.hpp>

namespace godot {

class Variant {
	friend class GDNative;

	static void init_bindings();

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

}

#endif // ! GODOT_CPP_VARIANT_HPP
