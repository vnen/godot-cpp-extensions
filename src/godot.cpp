#include "godot.hpp"

#include "core/class_db.hpp"

namespace godot {

const GDNativeInterface *interface;
GDNativeExtensionClassLibraryPtr library;

GDNativeBool GDNative::init(const GDNativeInterface *p_interface, const GDNativeExtensionClassLibraryPtr p_library, GDNativeInitialization *r_initialization) {
    interface = p_interface;
    library = p_library;

    r_initialization->initialize = initialize_level;
    r_initialization->deinitialize = deinitialize_level;

    return true;
}

void GDNative::initialize_level(void *userdata, GDNativeInitializationLevel p_level) {
    ClassDB::initialize(p_level);
}

void GDNative::deinitialize_level(void *userdata, GDNativeInitializationLevel p_level) {
    ClassDB::deinitialize(p_level);
}

}
