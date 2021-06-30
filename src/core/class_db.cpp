#include "class_db.hpp"

#include "godot.hpp"

namespace godot {

std::unordered_map<const char *, ClassDB::ClassInfo> ClassDB::classes;

MethodDefinition D_METHOD(const char *p_name) {
    return MethodDefinition(p_name);
}

MethodDefinition D_METHOD(const char *p_name, const char *p_arg1) {
    MethodDefinition method(p_name);
    method.args.push_back(p_arg1);
    return method;
}

MethodDefinition D_METHOD(const char *p_name, const char *p_arg1, const char *p_arg2) {
    MethodDefinition method(p_name);
    method.args.push_back(p_arg1);
    method.args.push_back(p_arg2);
    return method;
}

MethodDefinition D_METHOD(const char *p_name, const char *p_arg1, const char *p_arg2, const char *p_arg3) {
    MethodDefinition method(p_name);
    method.args.push_back(p_arg1);
    method.args.push_back(p_arg2);
    method.args.push_back(p_arg3);
    return method;
}

void ClassDB::initialize(GDNativeInitializationLevel p_level) {
    for (const std::pair<const char *, ClassInfo> pair : classes) {
        const ClassInfo &cl = pair.second;
        if (cl.level != p_level) {
            continue;
        }

        GDNativeExtensionClassCreationInfo class_info = {
            nullptr, // GDNativeExtensionClassSet set_func;
            nullptr, // GDNativeExtensionClassGet get_func;
            nullptr, // GDNativeExtensionClassGetPropertyList get_property_list_func;
            nullptr, // GDNativeExtensionClassFreePropertyList free_property_list_func;
            nullptr, // GDNativeExtensionClassNotification notification_func;
            nullptr, // GDNativeExtensionClassToString to_string_func;
            nullptr, // GDNativeExtensionClassReference reference_func;
            nullptr, // GDNativeExtensionClassUnreference
            cl.constructor, // GDNativeExtensionClassCreateInstance create_instance_func; /* this one is mandatory */
            cl.destructor, // GDNativeExtensionClassFreeInstance free_instance_func; /* this one is mandatory */
            nullptr, // GDNativeExtensionClassGetVirtual get_virtual_func;
        };

        interface->classdb_register_extension_class(library, cl.name, cl.parent_name, &class_info);

        for (MethodBind *method : cl.method_order) {
            GDNativeExtensionClassMethodInfo method_info = {
                method->get_name(), //const char *name;
                method, //void *method_userdata;
                MethodBind::bind_call, //GDNativeExtensionClassMethodCall call_func;
                MethodBind::bind_ptrcall, //GDNativeExtensionClassMethodPtrCall ptrcall_func;
                GDNATIVE_EXTENSION_METHOD_FLAGS_DEFAULT,//uint32_t method_flags; /* GDNativeExtensionClassMethodFlags */
                (uint32_t)method->get_argument_count(), //uint32_t argument_count;
                (GDNativeBool)method->has_return(), //GDNativeBool has_return_value;
                MethodBind::bind_get_argument_type, //(GDNativeExtensionClassMethodGetArgumentType) get_argument_type_func;
                MethodBind::bind_get_argument_info, //GDNativeExtensionClassMethodGetArgumentInfo get_argument_info_func; /* name and hint information for the argument can be omitted in release builds. Class name should always be present if it applies. */
                MethodBind::bind_get_argument_metadata, //GDNativeExtensionClassMethodGetArgumentMetadata get_argument_metadata_func;
                0, //uint32_t default_argument_count;
                nullptr, //GDNativeVariantPtr *default_arguments;
            };
            interface->classdb_register_extension_class_method(library, cl.name, &method_info);
        }
    }
}

void ClassDB::deinitialize(GDNativeInitializationLevel p_level) {
    for (const std::pair<const char *, ClassInfo> pair : classes) {
        const ClassInfo &cl = pair.second;
        if (cl.level != p_level) {
            continue;
        }

        interface->classdb_unregister_extension_class(library, cl.name);
    }
}

}
