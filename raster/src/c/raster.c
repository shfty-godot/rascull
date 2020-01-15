#include "raster.h"

#include "lib_raster.h"
#include "vector.h"
#include "matrix.h"
#include "raster_math.h"
#include "raster_geometry.h"

#include <math.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>

//#define WIREFRAME

const ivec2 resolution = {256, 128};

int get_pixel_count()
{
    return resolution.x * resolution.y;
}

void clear_depth_buffer(void *p_user_data)
{
    user_data_struct *user_data = (user_data_struct *)p_user_data;
    memset(user_data->depth_buffer, 0, get_pixel_count() * sizeof(int));
}

inline void render_pixel(void *p_user_data, int x, int y, int depth)
{
    user_data_struct *user_data = (user_data_struct *)p_user_data;
    user_data->depth_buffer[x + (y * 256)] = depth;
}

int bresenham_line(void *p_user_data, void *p_points, ivec2 v0, ivec2 v1)
{
    user_data_struct *user_data = (user_data_struct *)p_user_data;
    int *points = (int *)p_points;

    bool changed = false;

    int x = v0.x;
    int y = v0.y;

    int dx = v1.x - v0.x;
    int dy = v1.y - v0.y;

    int len_x = abs(dx);
    int len_y = abs(dy);

    int sign_x = sign(dx);
    int sign_y = sign(dy);

    if (len_y > len_x)
    {
        swap(&len_x, &len_y);
        changed = true;
    }

    int head = 0;

    int e = 2 * len_y - len_x;
    for (int i = 0; i < len_x; ++i)
    {
        if (points != NULL)
        {
            points[head] = x;
        }

        render_pixel(p_user_data, x, y, 100);

        while (e >= 0)
        {
            if (changed)
            {
                x += sign_x;
            }
            else
            {
                y += sign_y;
                head += 1;
            }
            e -= 2 * len_x;
        }

        if (changed)
        {
            y += sign_y;
            head += 1;
        }
        else
        {
            x += sign_x;
        }
        e += 2 * len_y;
    }

    return head;
}

void bresenham_triangle(void *p_user_data, ivec2 v0, ivec2 v1, ivec2 v2)
{
    user_data_struct *user_data = (user_data_struct *)p_user_data;

    int *points_a;
    int *points_b;

    points_a = (int *)malloc(128 * sizeof(int));
    points_b = (int *)malloc(128 * sizeof(int));

    int point_count_a = bresenham_line(p_user_data, points_a, v0, v1);
    int point_count_b = bresenham_line(p_user_data, points_b, v0, v2);
    bresenham_line(p_user_data, NULL, v1, v2);

    int delta_y = v1.y - v0.y;
    int delta_sign = sign(delta_y);

#ifndef WIREFRAME
    for (int y = 0; y < point_count_a; ++y)
    {
        int ax = points_a[y];
        int bx = points_b[y];

        int min_x = MIN(ax, bx);
        min_x = MAX(min_x, 0);

        int max_x = MAX(ax, bx);
        max_x = MIN(max_x, resolution.x - 1);

        for (int x = min_x; x < max_x; ++x)
        {
            int row_y = delta_sign > 0 ? y + v0.y : v0.y - y;
            render_pixel(p_user_data, x, row_y, 100);
        }
    }
#endif

    free(points_a);
    free(points_b);
}

int vertical_sort(const void *a, const void *b)
{
    ivec2 *va = (ivec2 *)a;
    ivec2 *vb = (ivec2 *)b;
    return va->y - vb->y;
}

void bresenham_triangles(void *p_user_data, void *p_triangles, int vertex_count)
{
    ivec2 *triangles = (ivec2 *)p_triangles;

    for (int i = 0; i < vertex_count; i += 3)
    {
        ivec2 v0 = triangles[i];
        ivec2 v1 = triangles[i + 1];
        ivec2 v2 = triangles[i + 2];

        ivec2 t[] = {v0, v1, v2};
        qsort(t, 3, sizeof(ivec2), vertical_sort);

        v0 = t[0];
        v1 = t[1];
        v2 = t[2];

        if (v1.y == v2.y)
        {
            bresenham_triangle(p_user_data, v0, v1, v2);
        }
        else if (v0.y == v1.y)
        {
            bresenham_triangle(p_user_data, v2, v0, v1);
        }
        else
        {
            float f0[] = {(float)v0.x, (float)v0.y};
            float f1[] = {(float)v1.x, (float)v1.y};
            float f2[] = {(float)v2.x, (float)v2.y};
            
            ivec2 v3 = {(int)(f0[0] + ((f1[1] - f0[1]) / (f2[1] - f0[1])) * (f2[0] - f0[0])), v1.y};
            bresenham_triangle(p_user_data, v0, v1, v3);
            bresenham_triangle(p_user_data, v2, v1, v3);
        }
    }
}

void rasterize_triangles(void *p_user_data, void *p_triangles, int vertex_count)
{
    fvec3 *triangles = (fvec3 *)p_triangles;

    // Triangles to clip space
    mat4 proj_mat = projection_matrix(70.0f, 1024.0f / 600.0f, 0, 1, false);

    fvec3 *clip_triangles = malloc(vertex_count * sizeof(fvec3));
    for (int i = 0; i < vertex_count; ++i)
    {
        fvec3 v = triangles[i];
        fvec4 cv = mat4_mul_fvec4(proj_mat, (fvec4){v.x, v.y, v.z, 1.0});
        clip_triangles[i] = (fvec3){cv.x, cv.y, cv.z};
    }

    // Clip triangles
    fvec3 *clipped_triangles = (fvec3 *)malloc(0);
    int clipped_vertex_count = 0;

    for (int i = 0; i < vertex_count; i += 3)
    {
        fvec3 np = fvec3_normalize((fvec3){0.0f, 0.0f, -1.0f});
        fvec3 fp = fvec3_normalize((fvec3){0.0f, 0.0f, 1.0f});
        fvec3 rp = fvec3_normalize((fvec3){1.0f, 0.0f, -1.0f});
        fvec3 lp = fvec3_normalize((fvec3){-1.0f, 0.0f, -1.0f});
        fvec3 tp = fvec3_normalize((fvec3){0.0f, 1.0f, -1.0f});
        fvec3 bp = fvec3_normalize((fvec3){0.0f, -1.0f, -1.0f});

        fvec3 *clipped = malloc(sizeof(fvec3) * 3);
        clipped[0] = clip_triangles[i];
        clipped[1] = clip_triangles[i + 1];
        clipped[2] = clip_triangles[i + 2];

        int count = 3;
        clipped = clip_polygon(clipped, count, np, -0.05f, &count);
        clipped = clip_polygon(clipped, count, fp, 100.0f, &count);
        clipped = clip_polygon(clipped, count, lp, 0.0, &count);
        clipped = clip_polygon(clipped, count, rp, 0.0, &count);
        clipped = clip_polygon(clipped, count, tp, 0.0, &count);
        clipped = clip_polygon(clipped, count, bp, 0.0, &count);

        clipped_triangles = realloc(clipped_triangles, sizeof(fvec3) * (clipped_vertex_count + 3 * count));

        for (int vi = 1; vi < count - 1; ++vi)
        {
            clipped_triangles[clipped_vertex_count++] = clipped[0];
            clipped_triangles[clipped_vertex_count++] = clipped[vi];
            clipped_triangles[clipped_vertex_count++] = clipped[vi + 1];
        }

        free(clipped);
    }

    free(clip_triangles);

    // Perspective divide
    for (int i = 0; i < clipped_vertex_count; ++i)
    {
        clipped_triangles[i] = fvec3_div(clipped_triangles[i], (fvec3){clipped_triangles[i].z, clipped_triangles[i].z, 1.0});
    }

    // Convert to screen space
    ivec2 *screen_triangles = malloc(sizeof(ivec2) * clipped_vertex_count);
    for (int i = 0; i < clipped_vertex_count; ++i)
    {
        fvec3 vertex = clipped_triangles[i];
        vertex.y *= -1.0f;

        vertex.x += 1.0f;
        vertex.y += 1.0f;

        vertex.x *= 127.5f;
        vertex.y *= 63.5f;

        ivec2 screen_vertex = {(int)vertex.x, (int)vertex.y};
        screen_triangles[i] = screen_vertex;
    }

    free(clipped_triangles);

    clear_depth_buffer(p_user_data);
    bresenham_triangles(p_user_data, screen_triangles, clipped_vertex_count);

    free(screen_triangles);
}
