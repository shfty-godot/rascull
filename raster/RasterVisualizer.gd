extends Control

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
	var far_plane_inv = 1.0 / Raster.get_z_far()

	var res  = Raster.get_resolution()

	var depth_buffer = PoolByteArray()
	for depth in Raster.get_depth_buffer():
		depth_buffer.append((1.0 - (depth * far_plane_inv)) * 255.0)

	depth_image.create_from_data(res.x, res.y, false, Image.FORMAT_L8, depth_buffer)
	depth_texture.create_from_image(depth_image);
	update()

func _draw():
	if not depth_texture:
		return

	draw_texture(depth_texture, Vector2.ZERO)
