extends Camera

# TODO: Implement depth test functions for determining if triangles are visible

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
	# Gather
	var gather_start = OS.get_ticks_usec()
	var instances: Array = gather_instances()
	var instance_matrices_vertices: Array = get_instances_matrices_vertices_aabb(instances)
	var gather_time = OS.get_ticks_usec() - gather_start;

	# Prepare matrices
	var view_matrix = transform_to_matrix(global_transform)
	var inv_view_matrix = transform_to_matrix(global_transform.inverse())

	# Rasterize
	var rasterize_start = OS.get_ticks_usec()
	Raster.rasterize_objects(instance_matrices_vertices, view_matrix, inv_view_matrix)
	var rasterize_time = OS.get_ticks_usec() - rasterize_start;

	# Cull
	var cull_start = OS.get_ticks_usec()
	cull_instances(instances)
	var cull_time = OS.get_ticks_usec() - cull_start;

	Profiler.set_timestamp("gather", gather_time)
	Profiler.set_timestamp("rasterize", rasterize_time)
	Profiler.set_timestamp("cull", cull_time)

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

func get_instances_matrices_vertices(objects) -> Array:
	var object_matrices_vertices := []
	for child in objects:
		if child is MeshInstance:
			object_matrices_vertices.append([
				transform_to_matrix(child.global_transform),
				transform_to_matrix(child.global_transform.inverse()),
				child.get_mesh().get_faces()
			])

	return object_matrices_vertices

func get_instances_matrices_vertices_aabb(objects) -> Array:
	var object_matrices_vertices := []
	for child in objects:
		if child is MeshInstance:
			object_matrices_vertices.append([
				transform_to_matrix(child.global_transform),
				transform_to_matrix(child.global_transform.inverse()),
				instance_aabb_to_triangles(child)
			])

	return object_matrices_vertices

func instance_aabb_to_triangles(instance: VisualInstance) -> PoolVector3Array:
	var aabb = instance.get_aabb()
	var triangles = PoolVector3Array()

	var vertices = PoolVector3Array()
	for i in range(0, 8):
		vertices.append(aabb.get_endpoint(i))

	# -X face
	triangles.append(vertices[2])
	triangles.append(vertices[1])
	triangles.append(vertices[0])

	triangles.append(vertices[1])
	triangles.append(vertices[2])
	triangles.append(vertices[3])

	# +X face
	triangles.append(vertices[5])
	triangles.append(vertices[6])
	triangles.append(vertices[4])

	triangles.append(vertices[6])
	triangles.append(vertices[5])
	triangles.append(vertices[7])

	# +Y face
	triangles.append(vertices[6])
	triangles.append(vertices[3])
	triangles.append(vertices[2])

	triangles.append(vertices[3])
	triangles.append(vertices[6])
	triangles.append(vertices[7])

	# -Y face
	triangles.append(vertices[1])
	triangles.append(vertices[4])
	triangles.append(vertices[0])

	triangles.append(vertices[4])
	triangles.append(vertices[1])
	triangles.append(vertices[5])

	# +Z face
	triangles.append(vertices[5])
	triangles.append(vertices[1])
	triangles.append(vertices[7])

	triangles.append(vertices[1])
	triangles.append(vertices[3])
	triangles.append(vertices[7])

	# -Z face
	triangles.append(vertices[0])
	triangles.append(vertices[4])
	triangles.append(vertices[6])

	triangles.append(vertices[2])
	triangles.append(vertices[0])
	triangles.append(vertices[6])

	return triangles

func transform_to_matrix(transform: Transform) -> PoolRealArray:
	var mat = PoolRealArray()

	for x in range(0, 4):
		for y in range(0, 3):
			mat.append(transform[x][y])
		mat.append(0 if x < 3 else 1)

	return mat

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
