extends Camera

export(NodePath) var cull_root_path
onready var cull_root = get_node(cull_root_path) if not cull_root_path.is_empty() else null

signal render_triangles(triangles)

var proj_mat = null

func _ready() -> void:
	update_projection(null)

func _process(delta) -> void:
	if not cull_root:
		return

	var objects: Array = gather_objects()
	var object_transforms_vertices: Array = get_object_transforms_vertices(objects)
	var front_face_transforms_vertices = cull_backface_transforms_vertices(object_transforms_vertices)
	var world_vertices = verts_to_world_space(front_face_transforms_vertices)
	var view_vertices: PoolVector3Array = verts_to_view_space(world_vertices)
	var clip_vertices_depths: Array = verts_to_clip_space(view_vertices)

	var clip_vertices = PoolVector3Array()
	var clip_depths = PoolRealArray()
	for cvd in clip_vertices_depths:
		clip_vertices.append(cvd[0])
		clip_depths.append(cvd[1])

	var clipped_vertices: PoolVector3Array = clip_verts(clip_vertices, clip_depths)
	emit_signal("render_triangles", clipped_vertices)

func update_projection(ignore):
	proj_mat = projection_matrix(get_fov(), get_viewport().size.x / get_viewport().size.y, 0, 1, get_keep_aspect_mode() == KEEP_WIDTH)

func projection_matrix(fovy_degrees: float, aspect: float, z_near: float, z_far: float, flip_fov: bool):
	if flip_fov:
		fovy_degrees = get_fovy(fovy_degrees, 1.0 / aspect)

	var radians = fovy_degrees / 2.0 * PI / 180.0

	var delta_z = z_far - z_near;

	var sine = sin(radians)

	if delta_z == 0 or sine == 0 or aspect == 0:
		return

	var cotangent = cos(radians) / sine

	return PoolRealArray([
		cotangent / aspect,		0,			0,									0,
		0,						cotangent,	0,									0,
		0,						0,			-(z_far + z_near) / delta_z,		1,
		0,						0,			-2 * z_near * z_far / delta_z,		0
	])

func get_fovy(fovx: float, aspect: float):
	return rad2deg(atan(aspect * tan(deg2rad(fovx) * 0.5)) * 2.0)

func mult_matrix(vec: Vector3, mat: PoolRealArray) -> PoolRealArray:
	var v = PoolRealArray([vec.x, vec.y, vec.z, 1.0])
	v[0] = mat[0] * v[0] + mat[4] * v[1] + mat[8]  * v[2] + mat[12] * v[3]
	v[1] = mat[1] * v[0] + mat[5] * v[1] + mat[9]  * v[2] + mat[13] * v[3]
	v[2] = mat[2] * v[0] + mat[6] * v[1] + mat[10] * v[2] + mat[14] * v[3]
	v[3] = mat[3] * v[0] + mat[7] * v[1] + mat[11] * v[2] + mat[15] * v[3]

	return v

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

func verts_to_clip_space(vertices: PoolVector3Array) -> Array:
	var clip_vertices = []

	for i in range(0, vertices.size()):
		var proj_vert = mult_matrix(vertices[i], proj_mat)
		clip_vertices.append([Vector3(proj_vert[0], proj_vert[1], proj_vert[2]), proj_vert[3]])

	return clip_vertices

func clip_verts(vertices: PoolVector3Array, depths: PoolRealArray) -> PoolVector3Array:
	var clipped_vertices = PoolVector3Array()

	for vert_idx in range(0, vertices.size(), 3):
		var v0 = vertices[vert_idx]
		v0.z = depths[vert_idx]

		var v1 = vertices[vert_idx + 1]
		v1.z = depths[vert_idx + 1]

		var v2 = vertices[vert_idx + 2]
		v2.z = depths[vert_idx + 2]

		var clipped_poly = PoolVector3Array([v0, v1, v2])
		clipped_poly = Geometry.clip_polygon(clipped_poly, Plane(Vector3.FORWARD, -get_znear()))
		clipped_poly = Geometry.clip_polygon(clipped_poly, Plane(Vector3.BACK, get_zfar()))
		clipped_poly = Geometry.clip_polygon(clipped_poly, Plane(Vector3(1, 0, -1), 0))
		clipped_poly = Geometry.clip_polygon(clipped_poly, Plane(Vector3(-1, 0, -1), 0))
		clipped_poly = Geometry.clip_polygon(clipped_poly, Plane(Vector3(0, 1, -1), 0))
		clipped_poly = Geometry.clip_polygon(clipped_poly, Plane(Vector3(0, -1, -1), 0))

		for clipped_vert_idx in range(1, clipped_poly.size() - 1):
			clipped_vertices.append(clipped_poly[0])
			clipped_vertices.append(clipped_poly[clipped_vert_idx])
			clipped_vertices.append(clipped_poly[clipped_vert_idx + 1])

	return clipped_vertices

func verts_to_screen_space(vertices: PoolVector3Array) -> PoolVector2Array:
	var screen_vertices = PoolVector2Array()
	for vertex in vertices:
		var screen_vertex = Vector2(vertex.x, vertex.y)
		screen_vertex.y *= -1
		screen_vertex += Vector2(1.0, 1.0)
		screen_vertex *= Vector2(127.5, 63.5) # Resolution + half pixel offset
		screen_vertices.append(screen_vertex)

	return screen_vertices