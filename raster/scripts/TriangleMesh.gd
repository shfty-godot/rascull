extends MeshInstance
tool

# Called when the node enters the scene tree for the first time.
func _init() -> void:
	if not Engine.editor_hint:
		return

	var mesh = ArrayMesh.new()
	var verts := []
	verts.resize(ArrayMesh.ARRAY_MAX)
	verts[ArrayMesh.ARRAY_VERTEX] = [
		Vector3(-0.5, 0.5, 0.0),
		Vector3(0.5, 0.5, 0),
		Vector3(-0.5, -0.5, 0)
	]

	mesh.add_surface_from_arrays(ArrayMesh.PRIMITIVE_TRIANGLES, verts)
	set_mesh(mesh)
