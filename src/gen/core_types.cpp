#include "core_types.hpp"

#include <src/godot.hpp>

namespace godot {

GDNativePtrConstructor String::string_copy_ptr;
GDNativePtrDestructor String::ptr_destructor;

void String::init_bindings() {
    string_copy_ptr = interface->variant_get_ptr_constructor(GDNATIVE_VARIANT_TYPE_STRING, 1);
    ptr_destructor = interface->variant_get_ptr_destructor(GDNATIVE_VARIANT_TYPE_STRING);
}

String::String() {
    interface->string_new_with_latin1_chars_and_len(&opaque, "", 0);
}

String::String(const char *p_latin_string) {
    interface->string_new_with_latin1_chars(&opaque, p_latin_string);
}

String::String(const String &p_string) {
    const GDNativeTypePtr args[1] = {
        (const GDNativeTypePtr)(&p_string),
    };
    string_copy_ptr(&opaque, args);
}

String::~String() {
    ptr_destructor(&opaque);
}

GDNativePtrConstructor NodePath::construct_from_nodepath;
GDNativePtrConstructor NodePath::construct_from_string;
GDNativePtrDestructor NodePath::ptr_destructor;

void NodePath::init_bindings() {
    construct_from_nodepath = interface->variant_get_ptr_constructor(GDNATIVE_VARIANT_TYPE_NODE_PATH, 1);
    construct_from_string = interface->variant_get_ptr_constructor(GDNATIVE_VARIANT_TYPE_NODE_PATH, 2);
    ptr_destructor = interface->variant_get_ptr_destructor(GDNATIVE_VARIANT_TYPE_NODE_PATH);
}

NodePath::NodePath(const char *p_path) : NodePath(String(p_path)) {}

NodePath::NodePath(const String &p_path) {
    const GDNativeTypePtr args[1] = {
        (const GDNativeTypePtr)(&p_path),
    };
    construct_from_string(&opaque, args);
}

NodePath::NodePath(const NodePath &p_path) {
    const GDNativeTypePtr args[1] = {
        (const GDNativeTypePtr)(&p_path),
    };
    construct_from_nodepath(&opaque, args);
}

NodePath::~NodePath() {
    ptr_destructor(&opaque);
}

}
