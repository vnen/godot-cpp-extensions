#include "engine_classes.hpp"

#include "core/defs.hpp"
#include "core/memory.hpp"
#include "godot.hpp"

#include <array>

namespace godot {

namespace internal {

template <class T, class... Args>
T _call_native_mb_ret(GDNativeMethodBindPtr mb, void *instance, Args... args) {
	T ret;
	std::array<void *, sizeof...(Args)> mb_args = { &args... };
	interface->object_method_bind_ptrcall(mb, instance, mb_args.data(), &ret);
	return ret;
}

template <class... Args>
void _call_native_mb_no_ret(GDNativeMethodBindPtr mb, void *instance, Args... args) {
	std::array<void *, sizeof...(Args)> mb_args = { &args... };
	interface->object_method_bind_ptrcall(mb, instance, mb_args.data(), nullptr);
}

}

Node *Node::get_node_internal(const NodePath &p_path) {
	static GDNativeMethodBindPtr mb = interface->classdb_get_method_bind("Node", "get_node", 135374120);
	return memnew(Node(internal::_call_native_mb_ret<GodotObject *>(mb, _owner, p_path)));
}

Node::Node() {
}

Node::Node(GodotObject *object) :
		Node() {
	_owner = object;
}

Node::~Node() {}

void Label::set_text(const String &p_text) {
	static GDNativeMethodBindPtr mb = interface->classdb_get_method_bind("Label", "set_text", 134188166);
    internal::_call_native_mb_no_ret(mb, _owner, p_text);
}

String Label::get_text() const {
	static GDNativeMethodBindPtr mb = interface->classdb_get_method_bind("Label", "get_text", 135338183);
    return internal::_call_native_mb_ret<String>(mb, _owner);
}

Label::Label() {
}

Label::Label(GodotObject *object) :
		Label() {
	_owner = object;
}

Label::~Label() {}

Button::Button() {
}

Button::Button(GodotObject *object) :
		Button() {
	_owner = object;
}

Button::~Button() {}

} // namespace godot
