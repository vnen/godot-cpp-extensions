#ifndef GODOT_TYPE_INFO_HPP
#define GODOT_TYPE_INFO_HPP

#include "object.hpp"
#include "variant/variant.hpp"

#include <godot-headers/gdnative_interface.h>

namespace godot {

// If the compiler fails because it's trying to instantiate the primary 'GetTypeInfo' template
// instead of one of the specializations, it's most likely because the type 'T' is not supported.
// If 'T' is a class that inherits 'Object', make sure it can see the actual class declaration
// instead of a forward declaration. You can always forward declare 'T' in a header file, and then
// include the actual declaration of 'T' in the source file where 'GetTypeInfo<T>' is instantiated.

template <class T, typename = void>
struct GetTypeInfo;

#define MAKE_TYPE_INFO(m_type, m_var_type)                                                                                     \
	template <>                                                                                                                \
	struct GetTypeInfo<m_type> {                                                                                               \
		static const GDNativeVariantType VARIANT_TYPE = m_var_type;                                                            \
		static const GDNativeExtensionClassMethodArgumentMetadata METADATA = GDNATIVE_EXTENSION_METHOD_ARGUMENT_METADATA_NONE; \
		static inline GDNativePropertyInfo get_class_info() {                                                                  \
			return PropertyInfo(VARIANT_TYPE, "");                                                                             \
		}                                                                                                                      \
	};                                                                                                                         \
	template <>                                                                                                                \
	struct GetTypeInfo<const m_type &> {                                                                                       \
		static const GDNativeVariantType VARIANT_TYPE = m_var_type;                                                            \
		static const GDNativeExtensionClassMethodArgumentMetadata METADATA = GDNATIVE_EXTENSION_METHOD_ARGUMENT_METADATA_NONE; \
		static inline GDNativePropertyInfo get_class_info() {                                                                  \
			return PropertyInfo(VARIANT_TYPE, "");                                                                             \
		}                                                                                                                      \
	};

#define MAKE_TYPE_INFO_WITH_META(m_type, m_var_type, m_metadata)                         \
	template <>                                                                          \
	struct GetTypeInfo<m_type> {                                                         \
		static const GDNativeVariantType VARIANT_TYPE = m_var_type;                      \
		static const GDNativeExtensionClassMethodArgumentMetadata METADATA = m_metadata; \
		static inline GDNativePropertyInfo get_class_info() {                            \
			return PropertyInfo(VARIANT_TYPE, "");                                       \
		}                                                                                \
	};                                                                                   \
	template <>                                                                          \
	struct GetTypeInfo<const m_type &> {                                                 \
		static const GDNativeVariantType VARIANT_TYPE = m_var_type;                      \
		static const GDNativeExtensionClassMethodArgumentMetadata METADATA = m_metadata; \
		static inline GDNativePropertyInfo get_class_info() {                            \
			return PropertyInfo(VARIANT_TYPE, "");                                       \
		}                                                                                \
	};

MAKE_TYPE_INFO(bool, GDNATIVE_VARIANT_TYPE_BOOL)
MAKE_TYPE_INFO_WITH_META(uint8_t, GDNATIVE_VARIANT_TYPE_INT, GDNATIVE_EXTENSION_METHOD_ARGUMENT_METADATA_INT_IS_UINT8)
MAKE_TYPE_INFO_WITH_META(int8_t, GDNATIVE_VARIANT_TYPE_INT, GDNATIVE_EXTENSION_METHOD_ARGUMENT_METADATA_INT_IS_INT8)
MAKE_TYPE_INFO_WITH_META(uint16_t, GDNATIVE_VARIANT_TYPE_INT, GDNATIVE_EXTENSION_METHOD_ARGUMENT_METADATA_INT_IS_UINT16)
MAKE_TYPE_INFO_WITH_META(int16_t, GDNATIVE_VARIANT_TYPE_INT, GDNATIVE_EXTENSION_METHOD_ARGUMENT_METADATA_INT_IS_INT16)
MAKE_TYPE_INFO_WITH_META(uint32_t, GDNATIVE_VARIANT_TYPE_INT, GDNATIVE_EXTENSION_METHOD_ARGUMENT_METADATA_INT_IS_UINT32)
MAKE_TYPE_INFO_WITH_META(int32_t, GDNATIVE_VARIANT_TYPE_INT, GDNATIVE_EXTENSION_METHOD_ARGUMENT_METADATA_INT_IS_INT32)
MAKE_TYPE_INFO_WITH_META(uint64_t, GDNATIVE_VARIANT_TYPE_INT, GDNATIVE_EXTENSION_METHOD_ARGUMENT_METADATA_INT_IS_UINT64)
MAKE_TYPE_INFO_WITH_META(int64_t, GDNATIVE_VARIANT_TYPE_INT, GDNATIVE_EXTENSION_METHOD_ARGUMENT_METADATA_INT_IS_INT64)
MAKE_TYPE_INFO(char16_t, GDNATIVE_VARIANT_TYPE_INT)
MAKE_TYPE_INFO(char32_t, GDNATIVE_VARIANT_TYPE_INT)
MAKE_TYPE_INFO_WITH_META(float, GDNATIVE_VARIANT_TYPE_FLOAT, GDNATIVE_EXTENSION_METHOD_ARGUMENT_METADATA_REAL_IS_FLOAT)
MAKE_TYPE_INFO_WITH_META(double, GDNATIVE_VARIANT_TYPE_FLOAT, GDNATIVE_EXTENSION_METHOD_ARGUMENT_METADATA_REAL_IS_DOUBLE)

MAKE_TYPE_INFO(String, GDNATIVE_VARIANT_TYPE_STRING)
MAKE_TYPE_INFO(Vector2, GDNATIVE_VARIANT_TYPE_VECTOR2)
MAKE_TYPE_INFO(Vector3, GDNATIVE_VARIANT_TYPE_VECTOR3)
MAKE_TYPE_INFO(Vector2i, GDNATIVE_VARIANT_TYPE_VECTOR2I)
MAKE_TYPE_INFO(Vector3i, GDNATIVE_VARIANT_TYPE_VECTOR3I)

} // namespace godot

#endif // ! GODOT_TYPE_INFO_HPP
