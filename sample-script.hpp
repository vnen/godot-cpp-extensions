#ifndef SAMPLE_SCRIPT_H
#define SAMPLE_SCRIPT_H

#include <src/core/class_db.hpp>
#include <src/core/object.hpp>
#include <src/core/variant.hpp>

#include "src/gen/engine_classes.hpp"

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
    void on_button_up();
    int get_x_from_vec(godot::Vector2i vec);

    int get_prop();
    void set_prop(int value);

    int method_with_many_params(int par1, int par2, int par3, int par4);

};

#endif // ! SAMPLE_SCRIPT_H
