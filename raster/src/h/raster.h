#ifndef RASTER_H
#define RASTER_H

#include <gdnative_api_struct.gen.h>

typedef struct ivec2 ivec2;

// Buffer management
int get_pixel_count();
void clear_depth_buffer();

// Rasterization
int bresenham_line(void* p_user_data, void* p_points, ivec2 v0, ivec2 v1);
void bresenham_triangle(void *p_user_data, ivec2 v0, ivec2 v1, ivec2 v2);
void bresenham_triangles(void *p_user_data, void *p_triangles, int vertex_count);
void rasterize_triangles(void *p_user_data, void *p_triangles, int vertex_count);

#endif
