#ifndef CLASS_DB_HPP
#define CLASS_DB_HPP

#include <godot-headers/gdnative_interface.h>

#include "defs.hpp"
#include "method_bind.hpp"

#include <list>
#include <unordered_map>
#include <string>
#include <vector>

namespace godot {

struct MethodDefinition {
    const char *name = nullptr;
	std::vector<std::string> args;
	MethodDefinition() {}
	MethodDefinition(const char *p_name) :
			name(p_name) {}
};

// TODO: Can this use variadic template?
MethodDefinition D_METHOD(const char *p_name);
MethodDefinition D_METHOD(const char *p_name, const char *p_arg1);
MethodDefinition D_METHOD(const char *p_name, const char *p_arg1, const char *p_arg2);
MethodDefinition D_METHOD(const char *p_name, const char *p_arg1, const char *p_arg2, const char *p_arg3);

class ClassDB {
public:
    struct ClassInfo {
        const char *name = nullptr;
        const char *parent_name = nullptr;
        GDNativeInitializationLevel level = GDNATIVE_INITIALIZATION_SCENE;
        std::unordered_map<const char *, MethodBind *> method_map;
        std::list<MethodBind *> method_order;
        GDExtensionClassInstancePtr (*constructor)(void *data);
        void (*destructor)(void *data, GDExtensionClassInstancePtr ptr);
    };

    static MethodBind *bind_methodfi(uint32_t p_flags, MethodBind *p_bind, const MethodDefinition &method_name, const void **p_defs, int p_defcount) {
        const char *instance_type = p_bind->get_instance_class();
        if (classes.find(instance_type) == classes.end()) {
            // TODO: Error message: class doesn't exist.
            memdelete(p_bind);
            return nullptr;
        }

        ClassInfo &type = classes[instance_type];

        if (type.method_map.find(method_name.name) != type.method_map.end()) {
            // TODO: Error message: duplicate method bind.
            memdelete(p_bind);
            return nullptr;
        }

        p_bind->set_name(method_name.name);

        if (method_name.args.size() > p_bind->get_argument_count()) {
            // TODO: Error message: method definition has more arguments than the actual method.
            memdelete(p_bind);
            return nullptr;
        }

        p_bind->set_argument_names(method_name.args);

        type.method_order.push_back(p_bind);
        type.method_map[method_name.name] = p_bind;

        return p_bind;
    }

private:
    static std::unordered_map<const char *, ClassInfo> classes;

public:
    template<class T>
    static void register_class(GDNativeInitializationLevel p_level = GDNATIVE_INITIALIZATION_SCENE);

    template<class N, class M>
    static MethodBind *bind_method(N p_method_name, M p_method);

    static void initialize(GDNativeInitializationLevel p_level);
    static void deinitialize(GDNativeInitializationLevel p_level);
};

template<class T>
void ClassDB::register_class(GDNativeInitializationLevel p_level) {
    ClassInfo cl;
    cl.name = T::get_class_static();
    cl.parent_name = T::get_parent_class_static();
    cl.level = p_level;
    cl.constructor = T::create;
    cl.destructor = T::free;
    classes[cl.name] = cl;
    T::initialize_class();
}

template<class N, class M>
MethodBind *ClassDB::bind_method(N p_method_name, M p_method) {
    MethodBind *bind = create_method_bind(p_method);

    return bind_methodfi(0, bind, p_method_name, nullptr, 0);
}
}

#endif // ! CLASS_DB_HPP
