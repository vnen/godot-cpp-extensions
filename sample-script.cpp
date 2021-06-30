#include "sample-script.hpp"

#include <godot-headers/gdnative_interface.h>
#include <src/core/defs.hpp>
#include <src/godot.hpp>

#include <iostream>

using namespace godot;

void MyScene::_bind_methods() {
    // TODO: Bind virtual methods.
    ClassDB::bind_method(D_METHOD("_ready"), &MyScene::_ready); // Probably needs to be different since it's a virtual, unless we magic that out via API information.
    ClassDB::bind_method(D_METHOD("on_button_down"), &MyScene::on_button_down);
    ClassDB::bind_method(D_METHOD("on_button_up"), &MyScene::on_button_up);
    ClassDB::bind_method(D_METHOD("get_x_from_vec", "vec"), &MyScene::get_x_from_vec);
}

void MyScene::_ready() {
    button = get_node<Button>("Button");
    label = get_node<Label>("Label");

    // button->connect("button_down", callable_mp(this, &MyScene::on_button_down));
    // button->connect("button_up", callable_mp(this, &MyScene::on_button_up));
    std::cout << "Ready called." << std::endl;
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
