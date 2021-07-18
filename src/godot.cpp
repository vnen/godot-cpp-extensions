#include "godot.hpp"

#include <core/class_db.hpp>
#include <core/memory.hpp>
#include <classes/wrapped.hpp>
#include <variant/variant.hpp>

#include <core/error_macros.hpp>

namespace godot {

namespace internal {

const GDNativeInterface *interface;
GDNativeExtensionClassLibraryPtr library;

}

GDNativeBool GDExtensionBinding::init(const GDNativeInterface *p_interface, const GDNativeExtensionClassLibraryPtr p_library, GDNativeInitialization *r_initialization) {
    internal::interface = p_interface;
    internal::library = p_library;

    r_initialization->initialize = initialize_level;
    r_initialization->deinitialize = deinitialize_level;

    Variant::init_bindings();

    return true;
}

void GDExtensionBinding::initialize_level(void *userdata, GDNativeInitializationLevel p_level) {
    ClassDB::initialize(p_level);
}

void GDExtensionBinding::deinitialize_level(void *userdata, GDNativeInitializationLevel p_level) {
    ClassDB::deinitialize(p_level);
}

void *GDExtensionBinding::create_instance_callback(void *p_token, void *p_instance) {
    ERR_FAIL_COND_V_MSG(p_token != internal::library, nullptr, "Asking for creating instance with invalid token.");
    Wrapped *wrapped = memnew(Wrapped(p_instance));
    return wrapped;
}

void GDExtensionBinding::free_instance_callback(void *p_token, void *p_instance, void *p_binding) {
    ERR_FAIL_COND_MSG(p_token != internal::library, "Asking for freeing instance with invalid token.");
    memdelete((Wrapped *)p_binding);
}

}
