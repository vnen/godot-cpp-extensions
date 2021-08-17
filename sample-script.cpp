/*************************************************************************/
/*  sample-script.cpp                                                    */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2021 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2021 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "sample-script.hpp"

#include <godot-headers/gdnative_interface.h>
#include <src/core/defs.hpp>
#include <src/core/object.hpp>
#include <src/godot.hpp>
#include <src/variant/variant.hpp>

#include <variant/utility_functions.hpp>

#include <iostream>

using namespace godot;

void MyScene::_bind_methods() {
	// TODO: Bind virtual methods.
	ClassDB::bind_method(D_METHOD("_ready"), &MyScene::_ready); // Probably needs to be different since it's a virtual, unless we magic that out via API information.
	ClassDB::bind_method(D_METHOD("on_button_down"), &MyScene::on_button_down);
	ClassDB::bind_method(D_METHOD("on_button_up"), &MyScene::on_button_up);
	ClassDB::bind_method(D_METHOD("get_x_from_vec", "vec"), &MyScene::get_x_from_vec);
	ClassDB::bind_method(D_METHOD("get_x_from_vec_no_const", "vec"), &MyScene::get_x_from_vec_no_const);
	ClassDB::bind_method(D_METHOD("get_y_from_vec", "vec"), &MyScene::get_y_from_vec);

	ClassDB::bind_method(D_METHOD("set_prop", "prop"), &MyScene::set_prop);
	ClassDB::bind_method(D_METHOD("get_prop"), &MyScene::get_prop);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "prop"), "set_prop", "get_prop");

	ClassDB::bind_method(D_METHOD("method_with_many_params", "par1", "par2", "par3", "par4"), &MyScene::method_with_many_params);

	{
		MethodInfo mi;
		mi.arguments.push_back(PropertyInfo(Variant::STRING, "some_argument"));
		mi.name = "varargs_func";
		ClassDB::bind_vararg_method(METHOD_FLAG_NORMAL, "varargs_func", &MyScene::varargs_func, mi, std::vector<Variant>{}, false);
	}

	ADD_SIGNAL(MethodInfo("my_signal", PropertyInfo(Variant::STRING, "name"), PropertyInfo(Variant::INT, "value")));

	BIND_CONSTANT(CONSTANT_WITHOUT_ENUM);

	BIND_ENUM_CONSTANT(FIRST_CONSTANT);
	BIND_ENUM_CONSTANT(ANSWER_TO_EVERYTHING);
}

void MyScene::_ready() {
	button = get_node<Button>("Button");
	label = get_node<Label>("Label");

	label->set_text("Ready was called!");

	// button->connect("button_down", callable_mp(this, &MyScene::on_button_down));
	// button->connect("button_up", callable_mp(this, &MyScene::on_button_up));
	std::cout << "Ready called." << std::endl;

	String txt = label->get_text();

	int size = internal::interface->string_to_utf8_chars(&txt, nullptr, 0);
	char txt_str[size + 1];
	internal::interface->string_to_utf8_chars(&txt, txt_str, size + 1);
	txt_str[size] = '\0';

	std::cout << "Read text: " << txt_str << std::endl;

	UtilityFunctions::prints("Test", "with", "multiple", "args");
	int64_t max = UtilityFunctions::max(50, 60, 120, 30);
	std::cout << "Max is " << max << std::endl;

	Label *new_label = memnew(Label);
	new_label->call("set_text", "Hey from call!");
	// new_label->set_text("Hey!");
	add_child(new_label);
}

void MyScene::on_button_down() {
	label->set_text("Button is pressed");
	std::cout << "Button down called." << std::endl;
}

void MyScene::on_button_up() const {
	label->set_text("Button is not pressed");
}

int MyScene::get_x_from_vec(const Vector2i &vec) const {
	return vec.get_x();
}

int MyScene::get_x_from_vec_no_const(const Vector2i &vec) {
	return vec.get_x();
}

int MyScene::get_y_from_vec(Vector2i vec) {
	return vec.get_y();
}

int MyScene::get_prop() {
	return prop;
}

void MyScene::set_prop(int value) {
	prop = value;
}

Variant MyScene::varargs_func(const Variant **args, GDNativeInt arg_count, GDNativeCallError &error) {
	UtilityFunctions::print("Varargs called with ", String::num(arg_count), " arguments");
	// error.error = GDNATIVE_CALL_OK;
	emit_signal("my_signal", "some_name", arg_count);
	return Variant();
}

int MyScene::method_with_many_params(int par1, int par2, int par3, int par4) {
	return par4;
}
