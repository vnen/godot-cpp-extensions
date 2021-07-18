#include <godot-headers/gdnative_interface.h>

#include <src/core/class_db.hpp>
#include <src/core/defs.hpp>
#include <src/godot.hpp>

#include "sample-script.hpp"

void register_classes() {
    godot::ClassDB::register_class<MyScene>();
}

extern "C" {

// Initialization.

GDNativeBool GDN_EXPORT sample_script_init(const GDNativeInterface *p_interface, const GDNativeExtensionClassLibraryPtr p_library, GDNativeInitialization *r_initialization) {
    GDNativeBool result = godot::GDExtensionBinding::init(p_interface, p_library, r_initialization);

    register_classes();

    return result;
}

void GDN_EXPORT initialize_level(void *userdata, GDNativeInitializationLevel p_level) {
    godot::GDExtensionBinding::initialize_level(userdata, p_level);
}

void GDN_EXPORT deinitialize_level(void *userdata, GDNativeInitializationLevel p_level) {
    godot::GDExtensionBinding::deinitialize_level(userdata, p_level);
}

}
