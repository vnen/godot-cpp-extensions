#include "engine_classes.hpp"

#include "core/defs.hpp"
#include "godot.hpp"

namespace godot {

Node *Node::get_node_internal(const NodePath &p_path) {
    static GDNativeMethodBindPtr mb = interface->classdb_get_method_bind("Node", "get_node", 135374120);
    GodotObject *result = nullptr;
    const GDNativeTypePtr args[1] = {
        (GDNativeTypePtr)&p_path,
    };
    interface->object_method_bind_ptrcall(mb, _owner, args, &result);
    return memnew(Node(result));
}

Node::Node() {
}

Node::Node(GodotObject *object) : Node() {
    _owner = object;
}

Node::~Node() {}

void Label::set_text(const String &p_text){
    static GDNativeMethodBindPtr mb = interface->classdb_get_method_bind("Label", "set_text", 134188166);
    const GDNativeTypePtr args[1] = {
        (GDNativeTypePtr)&p_text,
    };
    interface->object_method_bind_ptrcall(mb, _owner, args, nullptr);
}

Label::Label() {
}

Label::Label(GodotObject *object) : Label() {
    _owner = object;
}

Label::~Label() {}

Button::Button() {
}

Button::Button(GodotObject *object) : Button() {
    _owner = object;
}

Button::~Button() {}

}
