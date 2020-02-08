#ifndef RASTER_H
#define RASTER_H

#include <gdnative_api_struct.gen.h>

#include <vector.h>

ivec2 resolution;
bool flip_aspect;
float fov;
float aspect;
float z_near;
float z_far;

// Buffer management
void set_resolution(void *p_user_data, int x, int y);
int get_pixel_count();
void clear_depth_buffer();

// Triangle preparation
void verts_to_clip_space(fvec3 *p_triangles, int vertex_count);
fvec3 *clip_triangles(fvec3 *p_triangles, int *p_vertex_count);
void verts_to_screen_space(fvec3 *p_triangles, int vertex_count);
void verts_to_ndc(fvec3 *p_triangles, int vertex_count);

// Rasterization
bool bresenham_line(void *p_user_data, void *p_points, void *p_depths, fvec3 v0, fvec3 v1, bool depth_test, int *o_point_count);
bool bresenham_triangle(void *p_user_data, fvec3 v0, fvec3 v1, fvec3 v2, bool depth_test);
bool bresenham_triangles(void *p_user_data, void *p_triangles, int vertex_count, bool depth_test);

#endif
