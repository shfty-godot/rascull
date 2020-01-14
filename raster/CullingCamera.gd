extends Camera

export(NodePath) var cull_root_path
onready var cull_root = get_node(cull_root_path) if not cull_root_path.is_empty() else null

signal render_triangles(triangles)

onready var proj_mat = g_projection_matrix(get_fov(), get_viewport().size.x / get_viewport().size.y, get_znear(), get_zfar(), get_keep_aspect_mode() == KEEP_WIDTH)

func _process(delta) -> void:
	if not cull_root:
		return

	var vertices: PoolVector3Array = gather_vertices()

	var view_vertices: PoolVector3Array = verts_to_view_space(vertices)
	var clip_vertices: PoolVector3Array = verts_to_clip_space(view_vertices)
	var front_face_vertices: PoolVector3Array = cull_backfaces(clip_vertices)
	var clipped_vertices: PoolVector3Array = clip_verts(front_face_vertices)
	var screen_vertices: PoolVector2Array = verts_to_screen_space(clipped_vertices)

	emit_signal("render_triangles", screen_vertices)

func g_projection_matrix(fovy_degrees: float, aspect: float, z_near: float, z_far: float, flip_fov: bool):
	if flip_fov:
		fovy_degrees = get_fovy(fovy_degrees, 1.0 / aspect)

	var sine: float
	var cotangent: float
	var delta_z: float
	var radians = fovy_degrees / 2.0 * PI / 180.0

	delta_z = z_far - z_near
	sine = sin(radians)

	if delta_z == 0 or sine == 0 or aspect == 0:
		return

	cotangent = cos(radians) / sine

	return PoolRealArray([
		cotangent / aspect,		0,			0,								0,
		0,						cotangent,	0,								0,
		0,						0,			-(z_far + z_near) / delta_z,	1,
		0,						0,			-2 * z_near * z_far / delta_z,	0
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

func gather_vertices() -> PoolVector3Array:
	# Model space > world space
	var vertices = PoolVector3Array()
	for child in cull_root.get_children():
		if child is MeshInstance:
			var mesh = child.get_mesh()
			var triangles = mesh.get_faces()
			for vertex in triangles:
				vertices.append(child.global_transform.xform(vertex))

	return vertices

func verts_to_view_space(vertices: PoolVector3Array) -> PoolVector3Array:
	for i in range(0, vertices.size()):
		vertices[i] = global_transform.xform_inv(vertices[i])
	return vertices

func verts_to_clip_space(vertices: PoolVector3Array) -> PoolVector3Array:
	var view_size = get_viewport().size
	for i in range(0, vertices.size()):
		var proj_vert = mult_matrix(vertices[i], proj_mat)
		vertices[i] = Vector3(proj_vert[0], proj_vert[1], proj_vert[2]) / proj_vert[3]

	return vertices

func cull_backfaces(vertices: PoolVector3Array) -> PoolVector3Array:
	var front_face_vertices = PoolVector3Array()

	var vertex_count = vertices.size()
	for tri_idx in range(0, vertex_count, 3):
		var v0 = vertices[tri_idx]
		var v1 = vertices[tri_idx + 1]
		var v2 = vertices[tri_idx + 2]

		var dot = v0.cross(v1).dot((v2 - v0).normalized())
		if dot < 0:
			front_face_vertices.append(v0)
			front_face_vertices.append(v1)
			front_face_vertices.append(v2)

	return front_face_vertices

func clip_verts(vertices: PoolVector3Array) -> PoolVector3Array:
	var clipped_vertices = PoolVector3Array()

	for vert_idx in range(0, vertices.size(), 3):
		var v0 = vertices[vert_idx]
		var v1 = vertices[vert_idx + 1]
		var v2 = vertices[vert_idx + 2]

		var clipped_poly = PoolVector3Array([v0, v1, v2])
		clipped_poly = Geometry.clip_polygon(clipped_poly, Plane(Vector3.FORWARD, 1))
		clipped_poly = Geometry.clip_polygon(clipped_poly, Plane(Vector3.BACK, 1))
		clipped_poly = Geometry.clip_polygon(clipped_poly, Plane(Vector3.LEFT, 1))
		clipped_poly = Geometry.clip_polygon(clipped_poly, Plane(Vector3.RIGHT, 1))
		clipped_poly = Geometry.clip_polygon(clipped_poly, Plane(Vector3.UP, 1))
		clipped_poly = Geometry.clip_polygon(clipped_poly, Plane(Vector3.DOWN, 1))

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