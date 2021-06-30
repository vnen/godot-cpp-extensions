#ifndef GODOT_HPP
#define GODOT_HPP

#include <godot-headers/gdnative_interface.h>

namespace godot {

extern "C" const GDNativeInterface *interface;
extern "C" GDNativeExtensionClassLibraryPtr library;

class GDNative {
public:
    static GDNativeBool init(const GDNativeInterface *p_interface, const GDNativeExtensionClassLibraryPtr p_library, GDNativeInitialization *r_initialization);
    static void initialize_level(void *userdata, GDNativeInitializationLevel p_level);
    static void deinitialize_level(void *userdata, GDNativeInitializationLevel p_level);
};

}

#endif // ! GODOT_HPP
