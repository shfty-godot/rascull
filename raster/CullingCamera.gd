extends Camera

export(NodePath) var cull_root_path
onready var cull_root = get_node(cull_root_path) if not cull_root_path.is_empty() else null

signal render_triangles(triangles)

func _process(delta) -> void:
	if not cull_root:
		return

	var vertices = PoolVector3Array()
	for child in cull_root.get_children():
		var local_origin = global_transform.xform_inv(child.global_transform.origin)
		vertices.append(local_origin)

	var clipped_vertices = PoolVector3Array()
	for vert_idx in range(0, vertices.size(), 3):
		var v0 = vertices[vert_idx]
		var v1 = vertices[vert_idx + 1]
		var v2 = vertices[vert_idx + 2]

		var clipped_poly = PoolVector3Array([v0, v1, v2])
		clipped_poly = Geometry.clip_polygon(clipped_poly, Plane(Vector3.BACK, -2))
		clipped_poly = Geometry.clip_polygon(clipped_poly, Plane(Vector3.FORWARD, 8))
		clipped_poly = Geometry.clip_polygon(clipped_poly, Plane(Vector3.LEFT, 2))
		clipped_poly = Geometry.clip_polygon(clipped_poly, Plane(Vector3.RIGHT, 2))
		clipped_poly = Geometry.clip_polygon(clipped_poly, Plane(Vector3.UP, 2))
		clipped_poly = Geometry.clip_polygon(clipped_poly, Plane(Vector3.DOWN, 2))

		for clipped_vert_idx in range(1, clipped_poly.size() - 1):
			var c0 = clipped_poly[0]
			var c1 = clipped_poly[clipped_vert_idx]
			var c2 = clipped_poly[clipped_vert_idx + 1]
			clipped_vertices.append(c0)
			clipped_vertices.append(c1)
			clipped_vertices.append(c2)

	var screen_vertices = PoolVector2Array()
	for vertex in clipped_vertices:
		var viewport_rect = Rect2(Vector2.ZERO, get_viewport().size)
		vertex = unproject_position(vertex)
		vertex /= get_viewport().size
		vertex *= Vector2(256, 128)
		screen_vertices.append(vertex)

	emit_signal("render_triangles", screen_vertices)
