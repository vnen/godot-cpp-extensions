#ifndef GODOT_CLASSES_H
#define GODOT_CLASSES_H

// FIXME: Temporary. Putting all in on file because it's easier to handle.

#include "core_type.hpp"
#include "core/object.hpp"

namespace godot {

class Button;
class Label;
class Node2D;

class Node : public Object {
    GDCLASS(Node, Object);

protected:
    static void _bind_methods() {}

public:
    template<class T>
    T *get_node(const NodePath &p_pth) { return nullptr; }
};

class Node2D : public Node {
    GDCLASS(Node2D, Node);

protected:
    static void _bind_methods() {}
};

class Label : Node2D {
    GDCLASS(Label, Node2D);

protected:
    static void _bind_methods() {}

public:
    void set_text(const String &p_text) {}
};

}

#endif // ! GODOT_CLASSES_H
