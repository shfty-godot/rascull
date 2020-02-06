extends Camera

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
	var instance_ids: Array = gather_instance_ids()
	var occluders: Array = gather_instances(instance_ids, funcref(self, "should_gather_occluder"))
	var occludees: Array = gather_instances(instance_ids, funcref(self, "should_gather_occludee"))

	var instance_matrices_vertices: Array = get_instances_matrices_vertices(occluders)
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
	cull_instances(occluders, occludees, view_matrix, inv_view_matrix)
	var cull_time = OS.get_ticks_usec() - cull_start;

	Profiler.set_timestamp("gather", gather_time)
	Profiler.set_timestamp("rasterize", rasterize_time)
	Profiler.set_timestamp("cull", cull_time)

func gather_instance_ids() -> Array:
	return VisualServer.instances_cull_convex(get_frustum(), get_world().get_scenario())

func gather_instances(instance_ids: Array, gather_predicate: FuncRef) -> Array:
	var instances := []
	for instance_id in instance_ids:
		var instance: VisualInstance = instance_from_id(instance_id)
		if gather_predicate.call_func(instance):
			instances.append(instance)

	return instances

func should_gather_occluder(instance: VisualInstance) -> bool:
	return instance.is_in_group("occluder")

func should_gather_occludee(instance: VisualInstance) -> bool:
	return instance.is_in_group("occludee")

func get_instances_matrices_vertices(objects, aabb = false) -> Array:
	var object_matrices_vertices := []
	for child in objects:
		var matrices_vertices = [
			transform_to_matrix(child.global_transform),
			transform_to_matrix(child.global_transform.inverse())
		]

		if aabb:
			matrices_vertices.append(instance_aabb_to_triangles(child))
		else:
			if child is MeshInstance:
				matrices_vertices.append(get_meshinstance_vertices(child))
			elif child is CSGShape:
				matrices_vertices.append(get_csgshape_vertices(child))
			else:
				continue

		object_matrices_vertices.append(matrices_vertices)

	return object_matrices_vertices

func get_meshinstance_vertices(instance) -> Array:
	return instance.get_mesh().get_faces()

func get_csgshape_vertices(instance) -> Array:
	var mesh_array = instance.get_meshes()
	var mesh = mesh_array[1]
	var faces = mesh.get_faces()
	return faces

func instance_aabb_to_triangles(instance: VisualInstance) -> PoolVector3Array:
	var cube_mesh = CubeMesh.new()
	cube_mesh.set_size(instance.get_aabb().size)
	return cube_mesh.get_faces()

func transform_to_matrix(transform: Transform) -> PoolRealArray:
	var mat = PoolRealArray()

	for x in range(0, 4):
		for y in range(0, 3):
			mat.append(transform[x][y])
		mat.append(0 if x < 3 else 1)

	return mat

func cull_instances(occluders: Array, occludees: Array, view_matrix: PoolRealArray, inv_view_matrix: PoolRealArray) -> void:
	for node in get_tree().get_nodes_in_group("occluder"):
		node.visible = false

	for node in get_tree().get_nodes_in_group("occludee"):
		node.visible = false

	for instance in occluders:
		cull_instance(instance, view_matrix, inv_view_matrix)

	for instance in occludees:
		cull_instance(instance, view_matrix, inv_view_matrix)

func cull_instance(instance: VisualInstance, view_matrix: PoolRealArray, inv_view_matrix: PoolRealArray) -> void:
	if instance.get_transformed_aabb().has_point(global_transform.origin):
		instance.set_visible(true)
		return

	var matrices_vertices = get_instances_matrices_vertices([instance], true)
	var raster_visible = Raster.depth_test(matrices_vertices, view_matrix, inv_view_matrix)
	if raster_visible:
		instance.set_visible(true)
