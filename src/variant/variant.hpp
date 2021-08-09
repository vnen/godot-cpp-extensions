#ifndef GODOT_CPP_VARIANT_HPP
#define GODOT_CPP_VARIANT_HPP

#include <core/defs.hpp>

#include <variant/builtin_types.hpp>
#include <variant/variant_size.hpp>

#include <classes/object.hpp>

#include <godot-headers/gdnative_interface.h>

#include <array>

namespace godot {

class Variant {
	uint8_t opaque[GODOT_CPP_VARIANT_SIZE]{ 0 };
	GDNativeVariantPtr ptr = const_cast<uint8_t (*)[GODOT_CPP_VARIANT_SIZE]>(&opaque);

	friend class GDExtensionBinding;

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

	enum Operator {
		//comparison
		OP_EQUAL,
		OP_NOT_EQUAL,
		OP_LESS,
		OP_LESS_EQUAL,
		OP_GREATER,
		OP_GREATER_EQUAL,
		//mathematic
		OP_ADD,
		OP_SUBTRACT,
		OP_MULTIPLY,
		OP_DIVIDE,
		OP_NEGATE,
		OP_POSITIVE,
		OP_MODULE,
		//bitwise
		OP_SHIFT_LEFT,
		OP_SHIFT_RIGHT,
		OP_BIT_AND,
		OP_BIT_OR,
		OP_BIT_XOR,
		OP_BIT_NEGATE,
		//logic
		OP_AND,
		OP_OR,
		OP_XOR,
		OP_NOT,
		//containment
		OP_IN,
		OP_MAX
	};

private:
	static GDNativeVariantFromTypeConstructorFunc from_type_constructor[VARIANT_MAX];
	static GDNativeTypeFromVariantConstructorFunc to_type_constructor[VARIANT_MAX];

public:
	Variant();
	Variant(std::nullptr_t n) :
			Variant() {}
	Variant(const Variant &other);
	Variant(Variant &&other);
	Variant(bool v);
	Variant(int64_t v);
	Variant(int32_t v) :
			Variant(static_cast<int64_t>(v)) {}
	Variant(uint32_t v) :
			Variant(static_cast<int64_t>(v)) {}
	Variant(uint64_t v) :
			Variant(static_cast<int64_t>(v)) {}
	Variant(double v);
	Variant(float v) :
			Variant((double)v) {}
	Variant(const String &v);
	Variant(const char *v) : Variant(String(v)) {}
	Variant(const char16_t *v) : Variant(String(v)) {}
	Variant(const char32_t *v) : Variant(String(v)) {}
	Variant(const wchar_t *v) : Variant(String(v)) {}
	Variant(const Vector2 &v);
	Variant(const Vector2i &v);
	Variant(const Rect2 &v);
	Variant(const Rect2i &v);
	Variant(const Vector3 &v);
	Variant(const Vector3i &v);
	Variant(const Transform2D &v);
	Variant(const Plane &v);
	Variant(const Quaternion &v);
	Variant(const godot::AABB &v);
	Variant(const Basis &v);
	Variant(const Transform3D &v);
	Variant(const Color &v);
	Variant(const StringName &v);
	Variant(const NodePath &v);
	Variant(const godot::RID &v);
	Variant(const Object *v);
	Variant(const Callable &v);
	Variant(const Signal &v);
	Variant(const Dictionary &v);
	Variant(const Array &v);
	Variant(const PackedByteArray &v);
	Variant(const PackedInt32Array &v);
	Variant(const PackedInt64Array &v);
	Variant(const PackedFloat32Array &v);
	Variant(const PackedFloat64Array &v);
	Variant(const PackedStringArray &v);
	Variant(const PackedVector2Array &v);
	Variant(const PackedVector3Array &v);
	Variant(const PackedColorArray &v);
	~Variant();

	operator bool() const;
	operator int64_t() const;
	operator int32_t() const;
	operator uint64_t() const;
	operator uint32_t() const;
	operator double() const;
	operator float() const;
	operator String() const;
	operator Vector2() const;
	operator Vector2i() const;
	operator Rect2() const;
	operator Rect2i() const;
	operator Vector3() const;
	operator Vector3i() const;
	operator Transform2D() const;
	operator Plane() const;
	operator Quaternion() const;
	operator godot::AABB() const;
	operator Basis() const;
	operator Transform3D() const;
	operator Color() const;
	operator StringName() const;
	operator NodePath() const;
	operator godot::RID() const;
	operator Object *() const;
	operator Callable() const;
	operator Signal() const;
	operator Dictionary() const;
	operator Array() const;
	operator PackedByteArray() const;
	operator PackedInt32Array() const;
	operator PackedInt64Array() const;
	operator PackedFloat32Array() const;
	operator PackedFloat64Array() const;
	operator PackedStringArray() const;
	operator PackedVector2Array() const;
	operator PackedVector3Array() const;
	operator PackedColorArray() const;

	Variant &operator=(const Variant &other);
	Variant &operator=(Variant &&other);
	bool operator==(const Variant &other) const;
	bool operator!=(const Variant &other) const;
	bool operator<(const Variant &other) const;

	void call(const StringName &method, const Variant **args, int argcount, Variant &r_ret, GDNativeCallError &r_error);

	template<class ...Args>
	Variant call(const StringName &method, Args ...args) {
		Variant result;
		GDNativeCallError error;
		std::array<const GDNativeVariantPtr, sizeof...(Args)> call_args = { Variant(args)... };
		call(method, call_args.data(), call_args.size(), result, error);
		return result;
	}

	static void call_static(Variant::Type type, const StringName &method, const Variant **args, int argcount, Variant &r_ret, GDNativeCallError &r_error);

	template<class ...Args>
	static Variant call_static(Variant::Type type, const StringName &method, Args ...args) {
		Variant result;
		GDNativeCallError error;
		std::array<const GDNativeVariantPtr, sizeof...(Args)> call_args = { Variant(args)... };
		call_static(type, method, call_args.data(), call_args.size(), result, error);
		return result;
	}

	static void evaluate(const Operator &op, const Variant &a, const Variant &b, Variant &r_ret, bool &r_valid);

	void set(const Variant &key, const Variant &value, bool *r_valid = nullptr);
	void set_named(const StringName &name, const Variant &value, bool &r_valid);
	void set_indexed(int64_t index, const Variant &value, bool &r_valid, bool &r_oob);
	void set_keyed(const Variant &key, const Variant &value, bool &r_valid);
	Variant get(const Variant &key, bool *r_valid = nullptr) const;
	Variant get_named(const StringName &name, bool &r_valid) const;
	Variant get_indexed(int64_t index, bool &r_valid, bool &r_oob) const;
	Variant get_keyed(const Variant &key, bool &r_valid) const;
	bool in(const Variant &index, bool *r_valid = nullptr) const;

	bool iter_init(Variant &r_iter, bool &r_valid) const;
	bool iter_next(Variant &r_iter, bool &r_valid) const;
	Variant iter_get(const Variant &r_iter, bool &r_valid) const;

	Variant::Type get_type() const;
	bool has_method(const StringName &method) const;
	bool has_key(const Variant &key, bool *r_valid = nullptr) const;
	static bool has_member(Variant::Type type, const StringName &member);

	bool hash_compare(const Variant &variant) const;
	bool booleanize() const;
	String stringify() const;
	Variant duplicate(bool deep = false) const;
	static void blend(const Variant &a, const Variant &b, float c, Variant &r_dst);
	static void interpolate(const Variant &a, const Variant &b, float c, Variant &r_dst);

	static String get_type_name(Variant::Type type);
	static bool can_convert(Variant::Type from, Variant::Type to);
	static bool can_convert_strict(Variant::Type from, Variant::Type to);

	void clear();
};

} // namespace godot

#endif // ! GODOT_CPP_VARIANT_HPP
