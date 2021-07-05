#ifndef CLASS_DB_HPP
#define CLASS_DB_HPP

#include <godot-headers/gdnative_interface.h>

#include "defs.hpp"
#include "method_bind.hpp"
#include "object.hpp"

#include <list>
#include <unordered_map>
#include <string>
#include <vector>

namespace godot {

struct MethodDefinition {
    const char *name = nullptr;
	std::list<std::string> args;
	MethodDefinition() {}
	MethodDefinition(const char *p_name) :
			name(p_name) {}
};

MethodDefinition D_METHOD(const char *p_name);
MethodDefinition D_METHOD(const char *p_name, const char *p_arg1);
template <typename ...Args>
MethodDefinition D_METHOD(const char *p_name, const char *p_arg1, Args... args) {
    MethodDefinition md = D_METHOD(p_name, args...);
    md.args.push_front(p_arg1);
    return md;
}

class ClassDB {
public:
    struct PropertySetGet {
		int index;
		const char *setter;
		const char *getter;
		MethodBind *_setptr;
		MethodBind *_getptr;
		Variant::Type type;
	};

    struct ClassInfo {
        const char *name = nullptr;
        const char *parent_name = nullptr;
        GDNativeInitializationLevel level = GDNATIVE_INITIALIZATION_SCENE;
        std::unordered_map<const char *, MethodBind *> method_map;
        std::list<MethodBind *> method_order;
        GDExtensionClassInstancePtr (*constructor)(void *data);
        void (*destructor)(void *data, GDExtensionClassInstancePtr ptr);
        void (*object_instance)(GDExtensionClassInstancePtr p_instance, GDNativeObjectPtr p_object_instance);
        std::unordered_map<const char *, PropertySetGet> property_setget;
        std::list<PropertyInfo> property_list;
        ClassInfo *parent_ptr = nullptr;
    };

private:
    static std::unordered_map<const char *, ClassInfo> classes;

    static MethodBind *bind_methodfi(uint32_t p_flags, MethodBind *p_bind, const MethodDefinition &method_name, const void **p_defs, int p_defcount);

public:
    template<class T>
    static void register_class(GDNativeInitializationLevel p_level = GDNATIVE_INITIALIZATION_SCENE);

    template<class N, class M>
    static MethodBind *bind_method(N p_method_name, M p_method);
    static void add_property(const char *p_class, const PropertyInfo &p_pinfo, const char *p_setter, const char *p_getter, int p_index = -1);

    static MethodBind *get_method(const char *p_class, const char *p_method);

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
    cl.object_instance = T::set_object_instance;
    classes[cl.name] = cl;
    if (classes.find(cl.parent_name) != classes.end()) {
        cl.parent_ptr = &classes[cl.parent_name];
    }
    T::initialize_class();
}

template<class N, class M>
MethodBind *ClassDB::bind_method(N p_method_name, M p_method) {
    MethodBind *bind = create_method_bind(p_method);

    return bind_methodfi(0, bind, p_method_name, nullptr, 0);
}
}

#endif // ! CLASS_DB_HPP
