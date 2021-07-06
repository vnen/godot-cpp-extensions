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
protected:
    Node *get_node_internal(const NodePath &p_path);

public:
    // TODO: Do proper casting.
    template<class T>
    T *get_node(const NodePath &p_path) { return reinterpret_cast<T *>(get_node_internal(p_path)); }

    Node();
    Node(GodotObject *object);
    ~Node();
};

class Node2D : public Node {
};

class Label : Node2D {
public:
    void set_text(const String &p_text);

    Label();
    Label(GodotObject *object);
    ~Label();
};

class Button : Node2D {
public:
    Button();
    Button(GodotObject *object);
    ~Button();
};

}

#endif // ! GODOT_CLASSES_H
