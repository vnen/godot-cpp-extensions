/*************************************************************************/
/*  sample-script.hpp                                                    */
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

#ifndef SAMPLE_SCRIPT_H
#define SAMPLE_SCRIPT_H

#include <src/core/class_db.hpp>
#include <src/core/memory.hpp>
#include <src/core/object.hpp>
#include <src/variant/variant.hpp>

#include <classes/button.hpp>
#include <classes/label.hpp>
#include <classes/node2d.hpp>

// #include "src/gen/engine_classes.hpp"

class MyScene : public godot::Node2D {
	GDCLASS(MyScene, Node2D);

protected:
	static void _bind_methods();

private:
	godot::Button *button = nullptr;
	godot::Label *label = nullptr;

	int prop = 0;

public:
	void _ready();
	void on_button_down();
	void on_button_up() const;
	int get_x_from_vec(const godot::Vector2i &vec) const;
	int get_x_from_vec_no_const(const godot::Vector2i &vec);
	int get_y_from_vec(godot::Vector2i vec);

	int get_prop();
	void set_prop(int value);

	int method_with_many_params(int par1, int par2, int par3, int par4);

	MyScene() :
			Node2D(godot::internal::empty_constructor()) {}
};

#endif // ! SAMPLE_SCRIPT_H
