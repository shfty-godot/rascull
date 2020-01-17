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

// Rasterization
int bresenham_line(void* p_user_data, void* p_points, void* p_depths, fvec3 v0, fvec3 v1);
void bresenham_triangle(void *p_user_data, fvec3 v0, fvec3 v1, fvec3 v2);
void bresenham_triangles(void *p_user_data, void *p_triangles, int vertex_count);
void rasterize_triangles(void *p_user_data, void *p_triangles, int vertex_count);

#endif
