extends Node

signal rasterize_complete()
signal profile_timestamps(timestamps)

var timestamps = {}

func rasterize(triangles: PoolVector3Array) -> void:
	var ts = OS.get_ticks_usec()

	Raster.clear_depth_buffer()

	timestamps["clear"] = OS.get_ticks_usec() - ts

	ts = OS.get_ticks_usec()

	Raster.rasterize_triangles(triangles)

	timestamps["rasterize"] = OS.get_ticks_usec() - ts

	emit_signal("profile_timestamps", timestamps)
	emit_signal("rasterize_complete")
