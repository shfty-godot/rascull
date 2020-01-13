extends Spatial

var yaw = 0
var pitch = 0

func _ready() -> void:
	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)

func _physics_process(delta) -> void:
	var wish_vec = Vector3()
	wish_vec.x += -1 if Input.is_key_pressed(KEY_S) else 0
	wish_vec.x += 1 if Input.is_key_pressed(KEY_F) else 0
	wish_vec.y += 1 if Input.is_key_pressed(KEY_R) else 0
	wish_vec.y += -1 if Input.is_key_pressed(KEY_W) else 0
	wish_vec.z += -1 if Input.is_key_pressed(KEY_E) else 0
	wish_vec.z += 1 if Input.is_key_pressed(KEY_D) else 0

	translation += global_transform.basis.x * wish_vec.x * delta
	translation += Vector3.UP * wish_vec.y * delta
	translation += global_transform.basis.z * wish_vec.z * delta

	rotation = Vector3.ZERO
	rotate(Vector3.UP, yaw)
	rotate(global_transform.basis.x.normalized(), pitch)

func _input(event: InputEvent) -> void:
	if event is InputEventMouseMotion:
		yaw -= event.relative.x * 0.001
		pitch -= event.relative.y * 0.001
		pitch = clamp(pitch, deg2rad(-80), deg2rad(80))
