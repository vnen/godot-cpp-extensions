#include "variant.hpp"

#include <variant/string.hpp>
#include <variant/node_path.hpp>

namespace godot {

void Variant::init_bindings() {
    String::init_bindings();
    NodePath::init_bindings();
}

}
