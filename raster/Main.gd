extends Control

func rasterize():
	var start_ts = OS.get_ticks_usec()
	#Raster.bresenham_line(Vector2.ZERO, Vector2(64, 32))
	Raster.bresenham_triangle(Vector2(32, 32), Vector2(256 - 32, 128-32), Vector2(32, 128-32))
	print("Rasterize took " + String(OS.get_ticks_usec() - start_ts) + " usec")
