#include "class_db.hpp"

#include "error_macros.hpp"
#include "godot.hpp"

#include <algorithm>

namespace godot {

std::unordered_map<const char *, ClassDB::ClassInfo> ClassDB::classes;

MethodDefinition D_METHOD(const char *p_name) {
    return MethodDefinition(p_name);
}

MethodDefinition D_METHOD(const char *p_name, const char *p_arg1) {
    MethodDefinition method(p_name);
    method.args.push_front(p_arg1);
    return method;
}

void ClassDB::add_property(const char *p_class, const PropertyInfo &p_pinfo, const char *p_setter, const char *p_getter, int p_index) {
    ERR_FAIL_COND_MSG(classes.find(p_class) == classes.end(), "Trying to add property to non-existing class.");

    ClassInfo &info = classes[p_class];

    ERR_FAIL_COND_MSG(info.property_setget.find(p_pinfo.name) != info.property_setget.end(), "Property already exists in class.");

    MethodBind *setter = nullptr;
    if (p_setter) {
        setter = get_method(p_class, p_setter);

        ERR_FAIL_COND_MSG(!setter, "Setter method not found for property.");

        size_t exp_args = 1 + (p_index >= 0 ? 1 : 0);
        ERR_FAIL_COND_MSG(exp_args != setter->get_argument_count(), "Setter method must take a single argument.");
    }

    ERR_FAIL_COND_MSG(!p_getter, "Getter method must be specified.");

    MethodBind *getter = get_method(p_class, p_getter);
    ERR_FAIL_COND_MSG(!getter, "Getter method not found for property.");
    {
        size_t exp_args = 0 + (p_index >= 0 ? 1 : 0);
        ERR_FAIL_COND_MSG(exp_args != getter->get_argument_count(), "Getter method must not take any argument.");
    }

    info.property_list.push_back(p_pinfo);

    PropertySetGet setget;
    setget.setter = p_setter;
    setget.getter = p_getter;
    setget._setptr = setter;
    setget._getptr = getter;
    setget.index = p_index;
    setget.type = p_pinfo.type;

    info.property_setget[p_pinfo.name] = setget;
}

MethodBind *ClassDB::get_method(const char *p_class, const char *p_method) {
    ERR_FAIL_COND_V_MSG(classes.find(p_class) == classes.end(), nullptr, "Class not found.");

    ClassInfo *type = &classes[p_class];
    while (type) {
        std::unordered_map<const char *, MethodBind *>::iterator method = type->method_map.find(p_method);
        if (method != type->method_map.end()) {
            return method->second;
        }
        type = type->parent_ptr;
        continue;
    }

    return nullptr;
}

MethodBind *ClassDB::bind_methodfi(uint32_t p_flags, MethodBind *p_bind, const MethodDefinition &method_name, const void **p_defs, int p_defcount) {
        const char *instance_type = p_bind->get_instance_class();

        std::unordered_map<const char *, ClassInfo>::iterator type_it = classes.find(instance_type);
        if (type_it == classes.end()) {
            memdelete(p_bind);
            ERR_FAIL_V_MSG(nullptr, "Class doesn't exist.");
        }

        ClassInfo &type = type_it->second;

        if (type.method_map.find(method_name.name) != type.method_map.end()) {
            memdelete(p_bind);
            ERR_FAIL_V_MSG(nullptr, "Binding duplicate method.");
        }

        p_bind->set_name(method_name.name);

        if (method_name.args.size() > p_bind->get_argument_count()) {
            memdelete(p_bind);
            ERR_FAIL_V_MSG(nullptr, "Method definition has more arguments than the actual method.");
        }

        std::vector<std::string> args;
        args.resize(method_name.args.size());
        size_t arg_index = 0;
        for (std::string arg : method_name.args) {
            args[arg_index++] = arg;
        }

        p_bind->set_argument_names(args);

        type.method_order.push_back(p_bind);
        type.method_map[method_name.name] = p_bind;

        return p_bind;
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

        for (PropertyInfo property : cl.property_list) {
            GDNativePropertyInfo info = {
                (uint32_t)property.type, //uint32_t type;
                property.name,//const char *name;
                property.class_name, //const char *class_name;
                property.hint,// NONE //uint32_t hint;
                property.hint_string, // const char *hint_string;
                property.usage, // DEFAULT //uint32_t usage;
            };

            const PropertySetGet &setget = cl.property_setget.find(property.name)->second;

            interface->classdb_register_extension_class_property(library, cl.name, &info, setget.setter, setget.getter);
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
