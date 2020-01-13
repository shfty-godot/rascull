extends Label

func update_profiler(timestamps) -> void:
	var text = "Profile:\n"
	for timestamp_key in timestamps:
		text += timestamp_key + ": " + String(timestamps[timestamp_key]) + " usec\n"
	set_text(text)
