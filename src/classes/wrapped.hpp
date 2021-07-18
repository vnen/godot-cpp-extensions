#ifndef GODOT_CPP_WRAPPED_HPP
#define GODOT_CPP_WRAPPED_HPP

namespace godot {

typedef void GodotObject;

// Base for all engine classes, to contain the pointer to the engine instance.
class Wrapped {
    friend class GDExtensionBinding;

    // Private constructor, this should not be created directly by users.
	Wrapped(GodotObject *p_owner) :
			_owner(p_owner) {}

protected:
	GodotObject *_owner = nullptr;

	Wrapped() = default;
};

} // namespace godot

#endif // ! GODOT_CPP_WRAPPED_HPP
