#ifndef RASTER_H
#define RASTER_H

#include <gdnative_api_struct.gen.h>

typedef struct vec2 vec2;

// Buffer management
int get_pixel_count();
void clear_depth_buffer();

void render_pixel(void *p_user_data, int x, int y, int depth);

// Rasterization
int bresenham_line(void* p_user_data, void* p_points, vec2 v0, vec2 v1);
void bresenham_triangle(void *p_user_data, vec2 v0, vec2 v1, vec2 v2);
void rasterize_triangles(void *p_user_data, void *p_triangles, int vertex_count);

#endif
