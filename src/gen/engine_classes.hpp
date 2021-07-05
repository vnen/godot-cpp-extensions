#ifndef GODOT_CLASSES_H
#define GODOT_CLASSES_H

// FIXME: Temporary. Putting all in on file because it's easier to handle.

#include "core_types.hpp"
#include "core/object.hpp"

namespace godot {

class Button;
class Label;
class Node2D;

class Node : public Object {
    // GDCLASS(Node, Object);

    static GDNativeMethodBindPtr get_node_ptr;

    static bool initialized;
    static void init_bindings();

protected:
//     static void _bind_methods() {}
    Node *get_node_internal(const NodePath &p_path);

public:
    // TODO: Do proper casting.
    template<class T>
    T *get_node(const NodePath &p_path) { return memnew(T((GodotObject *)get_node_internal(p_path))); }

    Node();
    ~Node();
};

class Node2D : public Node {
    // GDCLASS(Node2D, Node);

// protected:
//     static void _bind_methods() {}
};

class Label : Node2D {
    // GDCLASS(Label, Node2D);

    static GDNativeMethodBindPtr set_text_ptr;

    static bool initialized;
    static void init_bindings();

// protected:
    // static void _bind_methods() {}

public:
    void set_text(const String &p_text);

    Label();
    Label(GodotObject *object);
    ~Label();
};

class Button : Node2D {
    static bool initialized;
    static void init_bindings();

public:
    Button();
    Button(GodotObject *object);
    ~Button();
};

}

#endif // ! GODOT_CLASSES_H
