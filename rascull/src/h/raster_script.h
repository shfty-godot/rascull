#ifndef RASTER_SCRIPT_H
#define RASTER_SCRIPT_H

#include <gdnative_api_struct.gen.h>

#define GODOT_DECLARE_FUNCTION(name)                                                   \
    godot_variant name(godot_object *p_instance, void *p_method_data, \
                                        void *p_user_data, int p_num_args, godot_variant **p_args)

void *raster_constructor(godot_object *p_instance, void *p_method_data);
void raster_destructor(godot_object *p_instance, void *p_method_data, void *p_user_data);

GODOT_DECLARE_FUNCTION(raster_get_resolution);
GODOT_DECLARE_FUNCTION(raster_get_z_near);
GODOT_DECLARE_FUNCTION(raster_get_z_far);
GODOT_DECLARE_FUNCTION(raster_set_resolution);
GODOT_DECLARE_FUNCTION(raster_set_flip_fov);
GODOT_DECLARE_FUNCTION(raster_set_fov);
GODOT_DECLARE_FUNCTION(raster_set_aspect);
GODOT_DECLARE_FUNCTION(raster_set_z_near);
GODOT_DECLARE_FUNCTION(raster_set_z_far);
GODOT_DECLARE_FUNCTION(raster_set_view_matrices);
GODOT_DECLARE_FUNCTION(raster_get_depth);
GODOT_DECLARE_FUNCTION(raster_get_depth_buffer);
GODOT_DECLARE_FUNCTION(raster_clear_depth_buffer);
GODOT_DECLARE_FUNCTION(raster_bresenham_line);
GODOT_DECLARE_FUNCTION(raster_bresenham_triangle);
GODOT_DECLARE_FUNCTION(raster_bresenham_triangles);
GODOT_DECLARE_FUNCTION(raster_rasterize_triangles);
GODOT_DECLARE_FUNCTION(raster_rasterize_objects);
GODOT_DECLARE_FUNCTION(raster_depth_test);

#endif
