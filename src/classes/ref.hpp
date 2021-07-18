#ifndef GODOT_CPP_REF_HPP
#define GODOT_CPP_REF_HPP

#include <core/defs.hpp>

#include <core/memory.hpp>
#include <classes/object.hpp>
#include <classes/ref_counted.hpp>
#include <variant/variant.hpp>

namespace godot {

// Helper class for RefCounted objects, same as Godot one.

class RefCounted;

template <class T>
class Ref {
	T *reference = nullptr;

	void ref(const Ref &p_from) {
		if (p_from.reference == reference) {
			return;
		}

		unref();

		reference = p_from.reference;
		if (reference) {
			reference->reference();
		}
	}

	void ref_pointer(T *p_ref) {
		ERR_FAIL_COND(!p_ref);

		if (p_ref->init_ref()) {
			reference = p_ref;
		}
	}

public:
	_FORCE_INLINE_ bool operator==(const T *p_ptr) const {
		return reference == p_ptr;
	}
	_FORCE_INLINE_ bool operator!=(const T *p_ptr) const {
		return reference != p_ptr;
	}

	_FORCE_INLINE_ bool operator<(const Ref<T> &p_r) const {
		return reference < p_r.reference;
	}
	_FORCE_INLINE_ bool operator==(const Ref<T> &p_r) const {
		return reference == p_r.reference;
	}
	_FORCE_INLINE_ bool operator!=(const Ref<T> &p_r) const {
		return reference != p_r.reference;
	}

	_FORCE_INLINE_ T *operator->() {
		return reference;
	}

	_FORCE_INLINE_ T *operator*() {
		return reference;
	}

	_FORCE_INLINE_ const T *operator->() const {
		return reference;
	}

	_FORCE_INLINE_ const T *ptr() const {
		return reference;
	}
	_FORCE_INLINE_ T *ptr() {
		return reference;
	}

	_FORCE_INLINE_ const T *operator*() const {
		return reference;
	}

	operator Variant() const {
		return Variant(reference);
	}

	void operator=(const Ref &p_from) {
		ref(p_from);
	}

	template <class T_Other>
	void operator=(const Ref<T_Other> &p_from) {
		RefCounted *refb = const_cast<RefCounted *>(static_cast<const RefCounted *>(p_from.ptr()));
		if (!refb) {
			unref();
			return;
		}
		Ref r;
		r.reference = Object::cast_to<T>(refb);
		ref(r);
		r.reference = nullptr;
	}

	void operator=(const Variant &p_variant) {
		// FIXME
		// Object *object = p_variant.get_validated_object();

		// if (object == reference) {
		// 	return;
		// }

		// unref();

		// if (!object) {
		// 	return;
		// }

		// T *r = Object::cast_to<T>(object);
		// if (r && r->reference()) {
		// 	reference = r;
		// }
	}

	template <class T_Other>
	void reference_ptr(T_Other *p_ptr) {
		if (reference == p_ptr) {
			return;
		}
		unref();

		T *r = Object::cast_to<T>(p_ptr);
		if (r) {
			ref_pointer(r);
		}
	}

	Ref(const Ref &p_from) {
		ref(p_from);
	}

	Ref(T *p_reference) {
		if (p_reference) {
			ref_pointer(p_reference);
		}
	}

	Ref(const Variant &p_variant) {
		// FIXME
		// Object *object = p_variant.get_validated_object();

		// if (!object) {
		// 	return;
		// }

		// T *r = Object::cast_to<T>(object);
		// if (r && r->reference()) {
		// 	reference = r;
		// }
	}

	inline bool is_valid() const { return reference != nullptr; }
	inline bool is_null() const { return reference == nullptr; }

	void unref() {
		if (reference && reference->unreference()) {
			memdelete(reference);
		}
		reference = nullptr;
	}

	void instantiate() {
		ref(memnew(T));
	}

	Ref() {}

	~Ref() {
		unref();
	}

	// Used exclusively in the bindings to recreate the Ref Godot encapsulates in return values,
	// without adding to the refcount.
	inline static Ref<T> ___internal_constructor(Object *obj) {
		Ref<T> r;
		r.reference = (T *)obj;
		return r;
	}
};

} // namespace godot

#endif // ! GODOT_CPP_REF_HPP
