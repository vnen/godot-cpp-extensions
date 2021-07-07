#include "sample-script.hpp"

#include <godot-headers/gdnative_interface.h>
#include <src/core/defs.hpp>
#include <src/core/object.hpp>
#include <src/core/variant.hpp>
#include <src/godot.hpp>

#include <iostream>

using namespace godot;

void MyScene::_bind_methods() {
    // TODO: Bind virtual methods.
    ClassDB::bind_method(D_METHOD("_ready"), &MyScene::_ready); // Probably needs to be different since it's a virtual, unless we magic that out via API information.
    ClassDB::bind_method(D_METHOD("on_button_down"), &MyScene::on_button_down);
    ClassDB::bind_method(D_METHOD("on_button_up"), &MyScene::on_button_up);
    ClassDB::bind_method(D_METHOD("get_x_from_vec", "vec"), &MyScene::get_x_from_vec);

    ClassDB::bind_method(D_METHOD("set_prop", "prop"), &MyScene::set_prop);
    ClassDB::bind_method(D_METHOD("get_prop"), &MyScene::get_prop);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "prop"), "set_prop", "get_prop");

    ClassDB::bind_method(D_METHOD("method_with_many_params", "par1", "par2", "par3", "par4"), &MyScene::method_with_many_params);
}

void MyScene::_ready() {
    button = get_node<Button>(String("Button"));
    label = get_node<Label>(String("Label"));

    label->set_text("Ready was called!");

    // button->connect("button_down", callable_mp(this, &MyScene::on_button_down));
    // button->connect("button_up", callable_mp(this, &MyScene::on_button_up));
    std::cout << "Ready called." << std::endl;

    String txt = label->get_text();

    int size = interface->string_to_utf8_chars(&txt, nullptr, 0);
    char txt_str[size + 1];
    interface->string_to_utf8_chars(&txt, txt_str, size + 1);
    txt_str[size] = '\0';

    std::cout << "Read text: " << txt_str << std::endl;

}

void MyScene::on_button_down() {
    label->set_text("Button is pressed");
    std::cout << "Button down called." << std::endl;
}

void MyScene::on_button_up() {
    label->set_text("Button is not pressed");
}

int MyScene::get_x_from_vec(Vector2i vec) {
    return vec.x;
}

int MyScene::get_prop() {
    return prop;
}

void MyScene::set_prop(int value) {
    prop = value;
}

int MyScene::method_with_many_params(int par1, int par2, int par3, int par4) {
    return par4;
}
