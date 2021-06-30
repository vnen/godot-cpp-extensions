#ifndef GODOT_CPP_MEMORY_HPP
#define GODOT_CPP_MEMORY_HPP

#include <stddef.h>

namespace godot {

class Memory {
	Memory();

public:
	static void *alloc_static(size_t p_bytes);
	static void *realloc_static(void *p_memory, size_t p_bytes);
	static void free_static(void *p_ptr);
};

#define memnew(m_v) new m_v
#define memdelete(m_v) delete (m_v)

}

#endif // ! GODOT_CPP_MEMORY_HPP
