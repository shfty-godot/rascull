extends Control

var depth_buffer = PoolIntArray()

var skip = 4
var current = 0

func render():
	if current == 0:
		depth_buffer = Raster.get_depth_buffer()
		update()

	current = (current + 1) % skip

func _draw():
	if not depth_buffer:
		return

	for x in range(0, 256):
		for y in range(0, 128):
			var depth = depth_buffer[x + (y * 256)]
			var color = Color(depth, depth, depth)
			draw_rect(Rect2(x, y, 1, 1), color)
