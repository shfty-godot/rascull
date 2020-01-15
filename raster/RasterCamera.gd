extends Camera

signal render_triangles(triangles)
signal rasterize_complete()
signal profile_timestamps(timestamps)

var proj_mat = null

func set_fov(new_fov) -> void:
	.set_fov(new_fov)
	# TODO: Update Raster FOV

func set_znear(new_znear) -> void:
	.set_znear(new_znear)
	# TODO: Update Raster ZNear

func set_zfar(new_zfar) -> void:
	.set_zfar(new_zfar)
	# TODO: Update Raster ZFar

# TODO: Hook Viewport::size_changed and update Raster aspect

# TODO: Move backface cull and world / view transformation into C

func _process(delta) -> void:
	var objects: Array = gather_objects()
	var object_transforms_vertices: Array = get_object_transforms_vertices(objects)
	var front_face_transforms_vertices: Array = cull_backface_transforms_vertices(object_transforms_vertices)
	var world_vertices: PoolVector3Array = verts_to_world_space(front_face_transforms_vertices)
	var view_vertices: PoolVector3Array = verts_to_view_space(world_vertices)

	# Rasterize
	var ts = OS.get_ticks_usec()
	Raster.rasterize_triangles(view_vertices)
	var es = OS.get_ticks_usec() - ts;
	emit_signal("profile_timestamps", {
		"rasterize": es
	})
	emit_signal("rasterize_complete")


func gather_objects() -> Array:
	var frustum_instance_ids = VisualServer.instances_cull_convex(get_frustum(), get_world().get_scenario())

	var instances := []
	for instance_id in frustum_instance_ids:
		var instance: VisualInstance = instance_from_id(instance_id)
		if should_gather_object(instance):
			instances.append(instance)

	return instances

func should_gather_object(instance: VisualInstance) -> bool:
	return instance.is_in_group("rasterize")

func get_object_transforms_vertices(objects) -> Array:
	var object_transforms_vertices := []
	for child in objects:
		if not child.visible:
			continue

		if child is MeshInstance:
			object_transforms_vertices.append({
				"transform": child.global_transform,
				"vertices": child.get_mesh().get_faces()
			})

	return object_transforms_vertices

func cull_backface_transforms_vertices(transforms_vertices: Array) -> Array:
	var front_face_transforms_vertices := []

	for transform_vertices in transforms_vertices:
		var transform = transform_vertices["transform"]
		var vertices = transform_vertices["vertices"]

		front_face_transforms_vertices.append({
			"transform": transform,
			"vertices": []
		})

		for tri_idx in range(0, vertices.size(), 3):
			var v0 = vertices[tri_idx]
			var v1 = vertices[tri_idx + 1]
			var v2 = vertices[tri_idx + 2]

			var vn = (v2 - v0).cross(v1 - v0).normalized()
			var local_camera = transform.xform_inv(global_transform.origin)
			var view_vector = (v0 - local_camera).normalized()

			if vn.dot(view_vector) <= 0:
				front_face_transforms_vertices[-1]["vertices"].append(v0)
				front_face_transforms_vertices[-1]["vertices"].append(v1)
				front_face_transforms_vertices[-1]["vertices"].append(v2)

	return front_face_transforms_vertices

func verts_to_world_space(transforms_vertices: Array) -> PoolVector3Array:
	var world_vertices = PoolVector3Array()

	for transform_vertices in transforms_vertices:
		var transform = transform_vertices["transform"]
		var vertices = transform_vertices["vertices"]
		for vertex in vertices:
			world_vertices.append(transform.xform(vertex))

	return world_vertices

func verts_to_view_space(vertices: PoolVector3Array) -> PoolVector3Array:
	for i in range(0, vertices.size()):
		vertices[i] = global_transform.xform_inv(vertices[i])
	return vertices
