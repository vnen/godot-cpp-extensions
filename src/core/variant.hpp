#ifndef GODOT_CPP_VARIANT_HPP
#define GODOT_CPP_VARIANT_HPP

#include "defs.hpp"

class Variant {};

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
