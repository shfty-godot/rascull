extends Control

signal fov_changed(fov)
signal z_near_changed(z_near)
signal z_far_changed(z_far)
signal keep_aspect_changed(horizontal)

func set_fov(fov):
	emit_signal("fov_changed", fov)

func set_z_near(z_near):
	emit_signal("z_near_changed", z_near)

func set_z_far(z_far):
	emit_signal("z_far_changed", z_far)

func set_flip_fov(flip_fov):
	emit_signal("keep_aspect_changed", Camera.KEEP_WIDTH if !flip_fov else Camera.KEEP_HEIGHT)
