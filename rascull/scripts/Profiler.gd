extends Node

var timestamps = {}

func _process(delta):
	set_timestamp("FPS", Performance.get_monitor(Performance.TIME_FPS), "FPS")
	set_timestamp("Frame Time", Performance.get_monitor(Performance.TIME_PROCESS) * 1000, "msec")
	set_timestamp("Object Count", Performance.get_monitor(Performance.RENDER_OBJECTS_IN_FRAME), "")
	set_timestamp("Draw Calls", Performance.get_monitor(Performance.RENDER_DRAW_CALLS_IN_FRAME), "")

func set_timestamp(key, timestamp, suffix):
	timestamps[key] = String(timestamp) + " " + suffix
