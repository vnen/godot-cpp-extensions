#include "engine_classes.hpp"

#include "core/defs.hpp"
#include "godot.hpp"

namespace godot {

bool Node::initialized = false;
GDNativeMethodBindPtr Node::get_node_ptr;

void Node::init_bindings() {
    initialized = true;
    get_node_ptr = interface->classdb_get_method_bind("Node", "get_node", 135374120);
}

Node *Node::get_node_internal(const NodePath &p_path) {
    Node *result = nullptr;
    const GDNativeTypePtr args[1] = {
        (GDNativeTypePtr)&p_path,
    };
    interface->object_method_bind_ptrcall(get_node_ptr, _owner, args, &result);
    return result;
}

Node::Node() {
    if (unlikely(!initialized)) {
        init_bindings();
    }
}

Node::~Node() {}

GDNativeMethodBindPtr Label::set_text_ptr;
bool Label::initialized = false;

void Label::init_bindings() {
    initialized = true;
    set_text_ptr = interface->classdb_get_method_bind("Label", "set_text", 134188166);
}

void Label::set_text(const String &p_text){
    const GDNativeTypePtr args[1] = {
        (GDNativeTypePtr)&p_text,
    };
    interface->object_method_bind_ptrcall(set_text_ptr, _owner, args, nullptr);
}

Label::Label() {
    if (unlikely(!initialized)) {
        init_bindings();
    }
}

Label::Label(GodotObject *object) : Label() {
    _owner = object;
}

Label::~Label() {}

bool Button::initialized = false;

void Button::init_bindings() {
    initialized = true;
}

Button::Button() {
    if (unlikely(!initialized)) {
        init_bindings();
    }
}

Button::Button(GodotObject *object) : Button() {
    _owner = object;
}

Button::~Button() {}

}
