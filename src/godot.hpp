#ifndef GODOT_HPP
#define GODOT_HPP

#include <godot-headers/gdnative_interface.h>

namespace godot {

namespace internal {

extern "C" const GDNativeInterface *interface;
extern "C" GDNativeExtensionClassLibraryPtr library;
extern "C" void *token;

}

class GDExtensionBinding {
public:
    static GDNativeBool init(const GDNativeInterface *p_interface, const GDNativeExtensionClassLibraryPtr p_library, GDNativeInitialization *r_initialization);
    static void initialize_level(void *userdata, GDNativeInitializationLevel p_level);
    static void deinitialize_level(void *userdata, GDNativeInitializationLevel p_level);

    static void *create_instance_callback(void *p_token, void *p_instance);
    static void free_instance_callback(void *p_token, void *p_instance, void *p_binding);
};

}

#endif // ! GODOT_HPP
