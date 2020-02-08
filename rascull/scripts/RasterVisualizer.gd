extends TextureRect

export(int) var frame_rate = 30 setget set_frame_rate

var timer = Timer.new()

var depth_image = Image.new()
var depth_texture = ImageTexture.new()

func set_frame_rate(new_frame_rate):
	if frame_rate != new_frame_rate:
		frame_rate = new_frame_rate
		update_timer()

func _ready():
	timer.connect("timeout", self, "render")
	add_child(timer)
	update_timer()

func update_timer():
	timer.stop()
	timer.set_wait_time(1.0 / frame_rate)
	timer.start()

func render():
	if not is_visible_in_tree():
		return

	var z_near = RasCull.get_z_near()
	var near_far_inv = 1.0 / (RasCull.get_z_far() - z_near)

	var res  = RasCull.get_resolution()

	var depth_buffer = PoolByteArray()
	for depth in RasCull.get_depth_buffer():
		var norm_depth = (depth - z_near) * near_far_inv
		var inv_depth = 1.0 - norm_depth
		depth_buffer.append(inv_depth * 255.0)

	depth_image.create_from_data(res.x, res.y, false, Image.FORMAT_L8, depth_buffer)
	depth_texture.create_from_image(depth_image, 0);
	set_texture(depth_texture)
