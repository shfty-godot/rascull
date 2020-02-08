class_name FrustumRenderer
extends MeshInstance
tool

var fov = 0;
var z_near = 0;
var z_far = 0;

func _process(delta) -> void:
	var camera := get_parent() as Camera

	if not camera:
		set_mesh(null)
		return

	var wants_update = false

	var camera_fov = camera.get_fov()
	if fov != camera_fov:
		fov = camera_fov
		wants_update = true

	var camera_z_near = camera.get_znear()
	if z_near != camera_z_near:
		z_near = camera_z_near
		wants_update = true

	var camera_z_far = camera.get_zfar()
	if z_far != camera_z_far:
		z_far = camera_z_far
		wants_update = true

	if wants_update:
		update_mesh(camera)

func update_mesh(camera: Camera):
	var planes = camera.get_frustum()

	var near_plane = planes[0]
	var far_plane = planes[1]
	var left_plane = planes[2]
	var up_plane = planes[3]
	var right_plane = planes[4]
	var down_plane = planes[5]

	var v0
	var v1
	var v2
	var v3

	var st = SurfaceTool.new()

	st.begin(Mesh.PRIMITIVE_TRIANGLES)

	# front face
	v0 = near_plane.intersect_3(left_plane, up_plane)
	v1 = near_plane.intersect_3(right_plane, up_plane)
	v2 = near_plane.intersect_3(right_plane, down_plane)
	v3 = near_plane.intersect_3(left_plane, down_plane)

	st.add_triangle_fan([v0, v1, v2, v3])

	# back face
	v0 = far_plane.intersect_3(right_plane, up_plane)
	v1 = far_plane.intersect_3(left_plane, up_plane)
	v2 = far_plane.intersect_3(left_plane, down_plane)
	v3 = far_plane.intersect_3(right_plane, down_plane)

	st.add_triangle_fan([v0, v1, v2, v3])

	# left face
	v0 = far_plane.intersect_3(left_plane, up_plane)
	v1 = near_plane.intersect_3(left_plane, up_plane)
	v2 = near_plane.intersect_3(left_plane, down_plane)
	v3 = far_plane.intersect_3(left_plane, down_plane)

	st.add_triangle_fan([v0, v1, v2, v3])

	# right face
	v0 = near_plane.intersect_3(right_plane, up_plane)
	v1 = far_plane.intersect_3(right_plane, up_plane)
	v2 = far_plane.intersect_3(right_plane, down_plane)
	v3 = near_plane.intersect_3(right_plane, down_plane)

	st.add_triangle_fan([v0, v1, v2, v3])

	# top face
	v0 = near_plane.intersect_3(left_plane, up_plane)
	v1 = far_plane.intersect_3(left_plane, up_plane)
	v2 = far_plane.intersect_3(right_plane, up_plane)
	v3 = near_plane.intersect_3(right_plane, up_plane)

	st.add_triangle_fan([v0, v1, v2, v3])

	# bottom face
	v0 = near_plane.intersect_3(right_plane, down_plane)
	v1 = far_plane.intersect_3(right_plane, down_plane)
	v2 = far_plane.intersect_3(left_plane, down_plane)
	v3 = near_plane.intersect_3(left_plane, down_plane)

	st.add_triangle_fan([v0, v1, v2, v3])

	set_mesh(st.commit())
