extends Label

func _process(delta) -> void:
	var text = "Profile:\n"
	for timestamp_key in Profiler.timestamps:
		text += timestamp_key + ": " + String(Profiler.timestamps[timestamp_key]) + "\n"
	set_text(text)
