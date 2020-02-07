class_name CameraController
extends Spatial

export(float) var move_speed = 5.0;

var yaw = 0
var pitch = 0

func _physics_process(delta) -> void:
	var mouse_held = Input.is_mouse_button_pressed(2)
	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED if mouse_held else Input.MOUSE_MODE_VISIBLE)

	var wish_vec = Vector3()
	wish_vec.x += -1 if Input.is_key_pressed(KEY_S) else 0
	wish_vec.x += 1 if Input.is_key_pressed(KEY_F) else 0
	wish_vec.y += 1 if Input.is_key_pressed(KEY_R) else 0
	wish_vec.y += -1 if Input.is_key_pressed(KEY_W) else 0
	wish_vec.z += -1 if Input.is_key_pressed(KEY_E) else 0
	wish_vec.z += 1 if Input.is_key_pressed(KEY_D) else 0

	translation += global_transform.basis * wish_vec * delta * move_speed

	rotation = Vector3.ZERO
	rotate(Vector3.UP, yaw)
	rotate(global_transform.basis.x.normalized(), pitch)

func _input(event: InputEvent) -> void:
	if event is InputEventMouseMotion and Input.is_mouse_button_pressed(2):
		yaw -= event.relative.x * 0.0015
		pitch -= event.relative.y * 0.0015
		pitch = clamp(pitch, deg2rad(-89.9), deg2rad(89.9))
	elif event is InputEventMouseButton:
		if event.button_index == BUTTON_WHEEL_UP:
			translation += global_transform.basis.z * -0.25;
		elif event.button_index == BUTTON_WHEEL_DOWN:
			translation += global_transform.basis.z * 0.25;
