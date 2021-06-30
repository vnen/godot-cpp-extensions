#ifndef GODOT_CORE_TYPES_HPP
#define GODOT_CORE_TYPES_HPP

// FIXME: Temporary. Putting all in on file because it's easier to handle.

namespace godot {

class String {
public:
    String(const char *) {}
};

class NodePath {
public:
    NodePath(const char *) {}
    NodePath(const String &) {}
};

}

#endif // ! GODOT_CORE_TYPES_HPP
