extends Camera

# TODO: Move backface cull into C (requires matrix inverse routines)
# TODO: Rasterize AABBs to check against depth buffer

export(int) var raster_x_resolution = 256 setget set_raster_x_resolution
export(int) var raster_y_resolution = 128 setget set_raster_y_resolution

func set_raster_x_resolution(new_raster_x_resolution):
	if new_raster_x_resolution != raster_x_resolution:
		raster_x_resolution = new_raster_x_resolution
		Raster.set_resolution(raster_x_resolution, raster_y_resolution)

func set_raster_y_resolution(new_raster_y_resolution):
	if new_raster_y_resolution != raster_y_resolution:
		raster_y_resolution = new_raster_y_resolution
		Raster.set_resolution(raster_x_resolution, raster_y_resolution)

func set_keep_aspect_mode(new_keep_aspect_mode) -> void:
	.set_keep_aspect_mode(new_keep_aspect_mode)
	print("set_keep_aspect_mode: ", new_keep_aspect_mode)
	Raster.set_flip_fov(new_keep_aspect_mode == Camera.KEEP_WIDTH)

func set_fov(new_fov) -> void:
	.set_fov(new_fov)
	Raster.set_fov(new_fov)

func set_znear(new_znear) -> void:
	.set_znear(new_znear)
	Raster.set_z_near(new_znear)

func set_zfar(new_zfar) -> void:
	.set_zfar(new_zfar)
	Raster.set_z_far(new_zfar)

func viewport_size_changed() -> void:
	var view_size = get_viewport().size
	Raster.set_aspect(view_size.x / view_size.y)

func _ready() -> void:
	get_viewport().connect("size_changed", self, "viewport_size_changed")
	Raster.set_resolution(raster_x_resolution, raster_y_resolution);
	Raster.set_flip_fov(get_keep_aspect_mode() == Camera.KEEP_WIDTH)
	Raster.set_fov(get_fov())
	Raster.set_z_near(get_znear())
	Raster.set_z_far(get_zfar())

	var view_size = get_viewport().size
	Raster.set_aspect(view_size.x / view_size.y)

func _process(delta) -> void:
	var instances: Array = gather_instances()
	var instance_matrices_vertices: Array = get_object_matrices_vertices(instances)
	var front_face_matrices_vertices: Array = cull_backface_matrices_vertices(instance_matrices_vertices)

	# Rasterize
	var ts = OS.get_ticks_usec()

	var matrices_vertices = []
	for matrix_vertices in front_face_matrices_vertices:
		matrices_vertices.append([
			matrix_vertices["matrix"],
			matrix_vertices["vertices"]
		]);

	Raster.rasterize_objects(matrices_vertices, transform_to_matrix(global_transform.inverse()))

	var es = OS.get_ticks_usec() - ts;

	cull_instances(instances)

	Profiler.set_timestamp("rasterize", es)

func gather_instances() -> Array:
	var frustum_instance_ids = VisualServer.instances_cull_convex(get_frustum(), get_world().get_scenario())

	var instances := []
	for instance_id in frustum_instance_ids:
		var instance: VisualInstance = instance_from_id(instance_id)
		if should_gather_object(instance):
			instances.append(instance)

	return instances

func should_gather_object(instance: VisualInstance) -> bool:
	return instance.is_in_group("occluder")

func get_object_matrices_vertices(objects) -> Array:
	var object_transforms_vertices := []
	for child in objects:
		if child is MeshInstance:
			var mat = transform_to_matrix(child.global_transform)
			object_transforms_vertices.append({
				"transform": child.global_transform,
				"matrix": mat,
				"vertices": child.get_mesh().get_faces()
			})

	return object_transforms_vertices

func transform_to_matrix(transform: Transform) -> PoolRealArray:
	var mat = PoolRealArray()

	for x in range(0, 4):
		for y in range(0, 3):
			mat.append(transform[x][y])
		mat.append(0 if x < 3 else 1)

	return mat

func cull_backface_matrices_vertices(transforms_vertices: Array) -> Array:
	var front_face_transforms_vertices := []

	for transform_vertices in transforms_vertices:
		var vertices = transform_vertices["vertices"]

		front_face_transforms_vertices.append({
			"vertices": [],
			"matrix": transform_vertices["matrix"]
		})

		for tri_idx in range(0, vertices.size(), 3):
			var v0 = vertices[tri_idx]
			var v1 = vertices[tri_idx + 1]
			var v2 = vertices[tri_idx + 2]

			var vn = (v2 - v0).cross(v1 - v0).normalized()
			var local_camera = transform_vertices["transform"].xform_inv(global_transform.origin)
			var view_vector = (v0 - local_camera).normalized()

			if vn.dot(view_vector) <= 0:
				front_face_transforms_vertices[-1]["vertices"].append(v0)
				front_face_transforms_vertices[-1]["vertices"].append(v1)
				front_face_transforms_vertices[-1]["vertices"].append(v2)

	return front_face_transforms_vertices

func cull_instances(instances: Array) -> void:
	for instance in instances:
		var aabb = instance.get_transformed_aabb()

		var aabb_center = Vector3.ZERO
		for i in range(0, 8):
			aabb_center += aabb.get_endpoint(i)
		aabb_center /= 8

		var screen_point = unproject_position(aabb_center)
		var ndc_point = screen_point / get_viewport().size
		var raster_point = ndc_point * Vector2(raster_x_resolution - 1, raster_y_resolution - 1)

		var x = clamp(floor(raster_point.x), 0, raster_x_resolution - 1)
		var y = clamp(floor(raster_point.y), 0, raster_y_resolution - 1)

		var depth = (aabb_center - global_transform.origin).length() - aabb.size.length() * 0.5
		var raster_depth = Raster.get_depth(x, y)

		instance.visible = depth < raster_depth
