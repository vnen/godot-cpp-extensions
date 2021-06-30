#include "memory.hpp"

#include "godot.hpp"

namespace godot {

void *Memory::alloc_static(size_t p_bytes) {
    return interface->mem_alloc(p_bytes);
}

void *Memory::realloc_static(void *p_memory, size_t p_bytes) {
    return interface->mem_realloc(p_memory, p_bytes);
}

void Memory::free_static(void *p_ptr) {
    interface->mem_free(p_ptr);
}

}
