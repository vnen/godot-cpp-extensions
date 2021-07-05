#ifndef GODOT_CPP_METHOD_PTRCALL_HPP
#define GODOT_CPP_METHOD_PTRCALL_HPP

#include "defs.hpp"

#include <gen/core_types.hpp>
#include "variant.hpp"

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

MAKE_PTRARGCONV(bool, uint32_t);
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

}

#endif // ! GODOT_CPP_METHOD_PTRCALL_HPP
