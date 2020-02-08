extends Camera

export(bool) var update = true

export(int) var raster_x_resolution = 256 setget set_raster_x_resolution
export(int) var raster_y_resolution = 128 setget set_raster_y_resolution

func set_raster_x_resolution(new_raster_x_resolution):
	if new_raster_x_resolution != raster_x_resolution:
		raster_x_resolution = new_raster_x_resolution
		RasCull.set_resolution(raster_x_resolution, raster_y_resolution)

func set_raster_y_resolution(new_raster_y_resolution):
	if new_raster_y_resolution != raster_y_resolution:
		raster_y_resolution = new_raster_y_resolution
		RasCull.set_resolution(raster_x_resolution, raster_y_resolution)

func set_keep_aspect_mode(new_keep_aspect_mode) -> void:
	.set_keep_aspect_mode(new_keep_aspect_mode)
	print("set_keep_aspect_mode: ", new_keep_aspect_mode)
	RasCull.set_flip_fov(new_keep_aspect_mode == Camera.KEEP_WIDTH)

func set_fov(new_fov) -> void:
	.set_fov(new_fov)
	RasCull.set_fov(new_fov)

func set_znear(new_znear) -> void:
	.set_znear(new_znear)
	RasCull.set_z_near(new_znear)

func set_zfar(new_zfar) -> void:
	.set_zfar(new_zfar)
	RasCull.set_z_far(new_zfar)

func set_update(new_update) -> void:
	if update != new_update:
		update = new_update

func viewport_size_changed() -> void:
	var view_size = get_viewport().size
	RasCull.set_aspect(view_size.x / view_size.y)

func _ready() -> void:
	get_viewport().connect("size_changed", self, "viewport_size_changed")
	RasCull.set_resolution(raster_x_resolution, raster_y_resolution);
	RasCull.set_flip_fov(get_keep_aspect_mode() == Camera.KEEP_WIDTH)
	RasCull.set_fov(get_fov())
	RasCull.set_z_near(get_znear())
	RasCull.set_z_far(get_zfar())

	var view_size = get_viewport().size
	RasCull.set_aspect(view_size.x / view_size.y)

func _process(delta) -> void:
	if not update:
		return

	# Gather
	var gather_start = OS.get_ticks_msec()
	var instance_ids: Array = gather_instance_ids()
	var occluders: Array = gather_instances(instance_ids, "occluder")
	var occludees: Array = gather_instances(instance_ids, "occludee")
	var gather_time = OS.get_ticks_msec() - gather_start;

	var gather_meshes_start = OS.get_ticks_msec()
	var mesh_matrices_vertices: Array = get_instances_matrices_vertices(occluders)
	var gather_meshes_time = OS.get_ticks_msec() - gather_meshes_start;

	var gather_aabbs_start = OS.get_ticks_msec()
	var aabb_matrices_vertices: Array = get_instances_matrices_aabbs(occluders + occludees)
	var gather_aabbs_time = OS.get_ticks_msec() - gather_aabbs_start;

	# Prepare matrices
	RasCull.set_view_matrices(
		transform_to_matrix(global_transform),
		transform_to_matrix(global_transform.inverse())
	)

	# Rasterize
	rasterize_objects(mesh_matrices_vertices)

	# Cull
	var cull_start = OS.get_ticks_msec()
	cull_instances(aabb_matrices_vertices)
	var cull_time = OS.get_ticks_msec() - cull_start;

	Profiler.set_timestamp("Gather Instances", gather_time, "msec")
	Profiler.set_timestamp("Gather Occluder Meshes", gather_meshes_time, "msec")
	Profiler.set_timestamp("Gather AABBs", gather_aabbs_time, "msec")
	Profiler.set_timestamp("Cull Occluded Objects", cull_time, "msec")

func gather_instance_ids() -> Array:
	return VisualServer.instances_cull_convex(get_frustum(), get_world().get_scenario())

func gather_instances(instance_ids: Array, group: String) -> Array:
	var instances := []
	for instance_id in instance_ids:
		var instance: VisualInstance = instance_from_id(instance_id)
		if instance.is_in_group(group):
			instances.append(instance)

	return instances

func get_instances_matrices_vertices(objects) -> Array:
	var object_matrices_vertices := []

	for child in objects:
		var trx = child.get_global_transform()

		var matrices_vertices = [
			child,
			transform_to_matrix(trx),
			transform_to_matrix(trx.inverse())
		]

		if child is MeshInstance:
			matrices_vertices.append(get_meshinstance_vertices(child))
		elif child is CSGShape:
			matrices_vertices.append(get_csgshape_vertices(child))
		else:
			continue

		object_matrices_vertices.append(matrices_vertices)

	return object_matrices_vertices

func get_instances_matrices_aabbs(objects) -> Array:
	var object_matrices_aabbs := []

	for child in objects:
		var trx = child.get_global_transform()
		object_matrices_aabbs.append([
			child,
			transform_to_matrix(trx),
			transform_to_matrix(trx.inverse()),
			instance_aabb_to_triangles(child)
		])

	return object_matrices_aabbs

func get_meshinstance_vertices(instance) -> Array:
	return instance.get_mesh().get_faces()

func get_csgshape_vertices(instance) -> Array:
	return instance.get_meshes()[1].get_faces()

func instance_aabb_to_triangles(instance: VisualInstance) -> PoolVector3Array:
	var aabb = instance.get_aabb()

	var endpoints = PoolVector3Array()
	for i in range(0, 8):
		endpoints.append(aabb.get_endpoint(i))

	return PoolVector3Array([
		# Back
		endpoints[2],
		endpoints[1],
		endpoints[0],

		endpoints[1],
		endpoints[2],
		endpoints[3],

		# Front
		endpoints[4],
		endpoints[5],
		endpoints[6],

		endpoints[7],
		endpoints[6],
		endpoints[5],

		# Bottom
		endpoints[5],
		endpoints[4],
		endpoints[0],

		endpoints[5],
		endpoints[0],
		endpoints[1],

		# Top
		endpoints[3],
		endpoints[6],
		endpoints[7],

		endpoints[2],
		endpoints[6],
		endpoints[3],

		# Left
		endpoints[7],
		endpoints[5],
		endpoints[3],

		endpoints[3],
		endpoints[5],
		endpoints[1],

		# Right
		endpoints[2],
		endpoints[4],
		endpoints[6],

		endpoints[4],
		endpoints[2],
		endpoints[0],
	])

func transform_to_matrix(transform: Transform) -> PoolRealArray:
	return PoolRealArray([
		transform[0][0],
		transform[0][1],
		transform[0][2],
		0,

		transform[1][0],
		transform[1][1],
		transform[1][2],
		0,

		transform[2][0],
		transform[2][1],
		transform[2][2],
		0,

		transform[3][0],
		transform[3][1],
		transform[3][2],
		1
	])

func rasterize_objects(occluder_mesh_matrices_vertices):
	var rasterize_start = OS.get_ticks_msec()
	RasCull.rasterize_objects(occluder_mesh_matrices_vertices)
	var rasterize_time = OS.get_ticks_msec() - rasterize_start;
	Profiler.set_timestamp("Rasterize Occluders", rasterize_time, "msec")

func cull_instances(instances_matrices_vertices: Array) -> void:
	var scene_tree = get_tree()

	scene_tree.call_group_flags(SceneTree.GROUP_CALL_REALTIME, "occluder", "set_visible", false)
	scene_tree.call_group_flags(SceneTree.GROUP_CALL_REALTIME, "occludee", "set_visible", false)

	for instance_matrices_vertices in instances_matrices_vertices:
		cull_instance(instance_matrices_vertices)

func cull_instance(instance_matrices_vertices: Array) -> void:
	var instance = instance_matrices_vertices[0]

	if instance.get_transformed_aabb().has_point(global_transform.origin):
		instance.set_visible(true)
		return

	if RasCull.depth_test([instance_matrices_vertices]):
		instance.set_visible(true)
