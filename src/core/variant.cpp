#include "variant.hpp"

namespace godot {

void Variant::init_bindings() {
    String::init_bindings();
    NodePath::init_bindings();
}

}
