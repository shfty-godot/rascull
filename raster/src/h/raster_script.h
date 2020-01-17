#ifndef RASTER_SCRIPT_H
#define RASTER_SCRIPT_H

#include <gdnative_api_struct.gen.h>

void *raster_constructor(godot_object *p_instance, void *p_method_data);
void raster_destructor(godot_object *p_instance, void *p_method_data, void *p_user_data);

godot_variant raster_get_resolution(godot_object *p_instance, void *p_method_data,
                                    void *p_user_data, int p_num_args, godot_variant **p_args);

godot_variant raster_get_z_far(godot_object *p_instance, void *p_method_data,
                               void *p_user_data, int p_num_args, godot_variant **p_args);

godot_variant raster_set_resolution(godot_object *p_instance, void *p_method_data,
                                    void *p_user_data, int p_num_args, godot_variant **p_args);

godot_variant raster_set_flip_fov(godot_object *p_instance, void *p_method_data,
                             void *p_user_data, int p_num_args, godot_variant **p_args);

godot_variant raster_set_fov(godot_object *p_instance, void *p_method_data,
                             void *p_user_data, int p_num_args, godot_variant **p_args);

godot_variant raster_set_aspect(godot_object *p_instance, void *p_method_data,
                                void *p_user_data, int p_num_args, godot_variant **p_args);

godot_variant raster_set_z_near(godot_object *p_instance, void *p_method_data,
                                void *p_user_data, int p_num_args, godot_variant **p_args);

godot_variant raster_set_z_far(godot_object *p_instance, void *p_method_data,
                               void *p_user_data, int p_num_args, godot_variant **p_args);

godot_variant raster_get_depth(godot_object *p_instance, void *p_method_data,
                               void *p_user_data, int p_num_args, godot_variant **p_args);

godot_variant raster_get_depth_buffer(godot_object *p_instance, void *p_method_data,
                                      void *p_user_data, int p_num_args, godot_variant **p_args);

godot_variant raster_clear_depth_buffer(godot_object *p_instance, void *p_method_data,
                                        void *p_user_data, int p_num_args, godot_variant **p_args);

godot_variant raster_bresenham_line(godot_object *p_instance, void *p_method_data,
                                    void *p_user_data, int p_num_args, godot_variant **p_args);

godot_variant raster_bresenham_triangle(godot_object *p_instance, void *p_method_data,
                                        void *p_user_data, int p_num_args, godot_variant **p_args);

godot_variant raster_bresenham_triangles(godot_object *p_instance, void *p_method_data,
                                         void *p_user_data, int p_num_args, godot_variant **p_args);

godot_variant raster_rasterize_triangles(godot_object *p_instance, void *p_method_data,
                                         void *p_user_data, int p_num_args, godot_variant **p_args);

godot_variant raster_rasterize_objects(godot_object *p_instance, void *p_method_data,
                                       void *p_user_data, int p_num_args, godot_variant **p_args);

#endif
