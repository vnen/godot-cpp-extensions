class_name MyScene

@onready var button: Button = $Button
@onready var label: Label = $Label

func _ready() -> void:
    button.button_down.connect(on_button_down)
    button.button_up.connect(on_button_up)

func on_button_down() -> void:
    label.text = "Button is pressed"

func on_button_up() -> void:
    label.text = "Button is not pressed"
