/*************************************************************************/
/*  memory.hpp                                                           */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2021 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2021 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#ifndef GODOT_CPP_MEMORY_HPP
#define GODOT_CPP_MEMORY_HPP

#include <cstddef>
#include <cstdint>

#include <core/defs.hpp>
#include <core/error_macros.hpp>

void *operator new(size_t p_size, const char *p_description); ///< operator new that takes a description and uses MemoryStaticPool
void *operator new(size_t p_size, void *p_pointer, size_t check, const char *p_description); ///< operator new that takes a description and uses a pointer to the preallocated memory

_ALWAYS_INLINE_ void *operator new(size_t p_size, void *p_pointer, size_t check, const char *p_description) {
	return p_pointer;
}

namespace godot {

class Memory {
	Memory();

public:
	static void *alloc_static(size_t p_bytes);
	static void *realloc_static(void *p_memory, size_t p_bytes);
	static void free_static(void *p_ptr);
};

#define memnew(m_v) (new ("") m_v)
#define memnew_placement(m_placement, m_class) (new (m_placement, sizeof(m_class), "") m_class)

template <class T>
void memdelete(T *p_class) {
	if (!__has_trivial_destructor(T)) {
		p_class->~T();
	}

	Memory::free_static(p_class);
}

#define memnew_arr(m_class, m_count) memnew_arr_template<m_class>(m_count)

template <typename T>
T *memnew_arr_template(size_t p_elements, const char *p_descr = "") {
	if (p_elements == 0) {
		return nullptr;
	}
	/** overloading operator new[] cannot be done , because it may not return the real allocated address (it may pad the 'element count' before the actual array). Because of that, it must be done by hand. This is the
	same strategy used by std::vector, and the Vector class, so it should be safe.*/

	size_t len = sizeof(T) * p_elements;
	uint64_t *mem = (uint64_t *)Memory::alloc_static(len);
	T *failptr = nullptr; // Get rid of a warning.
	ERR_FAIL_COND_V(!mem, failptr);
	*(mem - 1) = p_elements;

	if (!__has_trivial_constructor(T)) {
		T *elems = (T *)mem;

		/* call operator new */
		for (size_t i = 0; i < p_elements; i++) {
			new (&elems[i], sizeof(T), p_descr) T;
		}
	}

	return (T *)mem;
}

template <typename T>
void memdelete_arr(T *p_class) {
	uint64_t *ptr = (uint64_t *)p_class;

	if (!__has_trivial_destructor(T)) {
		uint64_t elem_count = *(ptr - 1);

		for (uint64_t i = 0; i < elem_count; i++) {
			p_class[i].~T();
		}
	}

	Memory::free_static(ptr);
}

} // namespace godot

#endif // ! GODOT_CPP_MEMORY_HPP
