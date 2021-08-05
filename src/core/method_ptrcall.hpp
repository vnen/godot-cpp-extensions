#ifndef GODOT_CPP_METHOD_PTRCALL_HPP
#define GODOT_CPP_METHOD_PTRCALL_HPP

#include <core/defs.hpp>

#include <variant/variant.hpp>
#include <godot.hpp>

namespace godot {

template <class T>
struct PtrToArg {};

#define MAKE_PTRARG(m_type)                                            \
	template <>                                                        \
	struct PtrToArg<m_type> {                                          \
		_FORCE_INLINE_ static m_type convert(const void *p_ptr) {      \
			return *reinterpret_cast<const m_type *>(p_ptr);           \
		}                                                              \
		typedef m_type EncodeT;                                        \
		_FORCE_INLINE_ static void encode(m_type p_val, void *p_ptr) { \
			*((m_type *)p_ptr) = p_val;                                \
		}                                                              \
	};                                                                 \
	template <>                                                        \
	struct PtrToArg<const m_type &> {                                  \
		_FORCE_INLINE_ static m_type convert(const void *p_ptr) {      \
			return *reinterpret_cast<const m_type *>(p_ptr);           \
		}                                                              \
		typedef m_type EncodeT;                                        \
		_FORCE_INLINE_ static void encode(m_type p_val, void *p_ptr) { \
			*((m_type *)p_ptr) = p_val;                                \
		}                                                              \
	}

#define MAKE_PTRARGCONV(m_type, m_conv)                                           \
	template <>                                                                   \
	struct PtrToArg<m_type> {                                                     \
		_FORCE_INLINE_ static m_type convert(const void *p_ptr) {                 \
			return static_cast<m_type>(*reinterpret_cast<const m_conv *>(p_ptr)); \
		}                                                                         \
		typedef m_conv EncodeT;                                                   \
		_FORCE_INLINE_ static void encode(m_type p_val, void *p_ptr) {            \
			*((m_conv *)p_ptr) = static_cast<m_conv>(p_val);                      \
		}                                                                         \
		_FORCE_INLINE_ static m_conv encode_arg(m_type p_val) {                   \
			return static_cast<m_conv>(p_val);                                    \
		}                                                                         \
	};                                                                            \
	template <>                                                                   \
	struct PtrToArg<const m_type &> {                                             \
		_FORCE_INLINE_ static m_type convert(const void *p_ptr) {                 \
			return static_cast<m_type>(*reinterpret_cast<const m_conv *>(p_ptr)); \
		}                                                                         \
		typedef m_conv EncodeT;                                                   \
		_FORCE_INLINE_ static void encode(m_type p_val, void *p_ptr) {            \
			*((m_conv *)p_ptr) = static_cast<m_conv>(p_val);                      \
		}                                                                         \
		_FORCE_INLINE_ static m_conv encode_arg(m_type p_val) {                   \
			return static_cast<m_conv>(p_val);                                    \
		}                                                                         \
	}

#define MAKE_PTRARG_BY_REFERENCE(m_type)                                      \
	template <>                                                               \
	struct PtrToArg<m_type> {                                                 \
		_FORCE_INLINE_ static m_type convert(const void *p_ptr) {             \
			return *reinterpret_cast<const m_type *>(p_ptr);                  \
		}                                                                     \
		typedef m_type EncodeT;                                               \
		_FORCE_INLINE_ static void encode(const m_type &p_val, void *p_ptr) { \
			*((m_type *)p_ptr) = p_val;                                       \
		}                                                                     \
	};                                                                        \
	template <>                                                               \
	struct PtrToArg<const m_type &> {                                         \
		_FORCE_INLINE_ static m_type convert(const void *p_ptr) {             \
			return *reinterpret_cast<const m_type *>(p_ptr);                  \
		}                                                                     \
		typedef m_type EncodeT;                                               \
		_FORCE_INLINE_ static void encode(const m_type &p_val, void *p_ptr) { \
			*((m_type *)p_ptr) = p_val;                                       \
		}                                                                     \
	}

MAKE_PTRARGCONV(bool, uint8_t);
// Integer types.
MAKE_PTRARGCONV(uint8_t, int64_t);
MAKE_PTRARGCONV(int8_t, int64_t);
MAKE_PTRARGCONV(uint16_t, int64_t);
MAKE_PTRARGCONV(int16_t, int64_t);
MAKE_PTRARGCONV(uint32_t, int64_t);
MAKE_PTRARGCONV(int32_t, int64_t);
MAKE_PTRARG(int64_t);
MAKE_PTRARG(uint64_t);
// Float types
MAKE_PTRARGCONV(float, double);
MAKE_PTRARG(double);

MAKE_PTRARG(String);
MAKE_PTRARG(Vector2);
MAKE_PTRARG(Vector2i);
MAKE_PTRARG(Rect2);
MAKE_PTRARG(Rect2i);
MAKE_PTRARG_BY_REFERENCE(Vector3);
MAKE_PTRARG_BY_REFERENCE(Vector3i);
MAKE_PTRARG(Transform2D);
MAKE_PTRARG_BY_REFERENCE(Plane);
MAKE_PTRARG(Quaternion);
MAKE_PTRARG_BY_REFERENCE(AABB);
MAKE_PTRARG_BY_REFERENCE(Basis);
MAKE_PTRARG_BY_REFERENCE(Transform3D);
MAKE_PTRARG_BY_REFERENCE(Color);
MAKE_PTRARG(StringName);
MAKE_PTRARG(NodePath);
MAKE_PTRARG(RID);
// Object doesn't need this.
MAKE_PTRARG(Callable);
MAKE_PTRARG(Signal);
MAKE_PTRARG(Dictionary);
MAKE_PTRARG(Array);
MAKE_PTRARG(PackedByteArray);
MAKE_PTRARG(PackedInt32Array);
MAKE_PTRARG(PackedInt64Array);
MAKE_PTRARG(PackedFloat32Array);
MAKE_PTRARG(PackedFloat64Array);
MAKE_PTRARG(PackedStringArray);
MAKE_PTRARG(PackedVector2Array);
MAKE_PTRARG(PackedVector3Array);
MAKE_PTRARG(PackedColorArray);
MAKE_PTRARG_BY_REFERENCE(Variant);

// This is for Object.

template <class T>
struct PtrToArg<T *> {
	_FORCE_INLINE_ static T *convert(const void *p_ptr) {
		return reinterpret_cast<T *>(godot::internal::interface->object_get_instance_binding(p_ptr, godot::internal::token, T::___binding_callbacks));
	}
	typedef Object *EncodeT;
	_FORCE_INLINE_ static void encode(T *p_var, void *p_ptr) {
		p_ptr = p_var->_owner;
	}
};

template <class T>
struct PtrToArg<const T *> {
	_FORCE_INLINE_ static const T *convert(const void *p_ptr) {
		return reinterpret_cast<const T *>(godot::internal::interface->object_get_instance_binding(p_ptr, godot::internal::token, T::___binding_callbacks));
	}
	typedef const Object *EncodeT;
	_FORCE_INLINE_ static void encode(T *p_var, void *p_ptr) {
		p_ptr = p_var->_owner;
	}
};

} // namespace godot

#endif // ! GODOT_CPP_METHOD_PTRCALL_HPP
