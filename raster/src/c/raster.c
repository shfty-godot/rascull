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

ivec2 resolution = {256, 128};
bool flip_aspect = false;
float fov = 70.0f;
float aspect = 1024.0f / 600.0f;
float z_near = 0.05f;
float z_far = 10.0f;

int get_pixel_count()
{
    return resolution.x * resolution.y;
}

void set_resolution(void *p_user_data, int x, int y)
{
    resolution.x = x;
    resolution.y = y;

    user_data_struct *user_data = (user_data_struct *)p_user_data;
    user_data->depth_buffer = realloc(user_data->depth_buffer, sizeof(float) * x * y);
}

void clear_depth_buffer(void *p_user_data)
{
    user_data_struct *user_data = (user_data_struct *)p_user_data;
    for (int i = 0; i < get_pixel_count(); ++i) {
        user_data->depth_buffer[i] = z_far;
    }
}

inline void render_pixel(void *p_user_data, int x, int y, float depth)
{
    user_data_struct *user_data = (user_data_struct *)p_user_data;
    int idx = x + (y * resolution.x);
    if(depth < user_data->depth_buffer[idx]) {
        user_data->depth_buffer[idx] = depth;
    }
}

int bresenham_line(void *p_user_data, void *p_points, void *p_depths, fvec3 v0, fvec3 v1)
{
    user_data_struct *user_data = (user_data_struct *)p_user_data;
    int *points = (int *)p_points;
    float *depths = (float *)p_depths;
    int head = 0;

    bool changed = false;

    int x = (int)v0.x;
    int y = (int)v0.y;

    int dx = (int)v1.x - x;
    int dy = (int)v1.y - y;

    int len_x = abs(dx);
    int len_y = abs(dy);

    int sign_x = sign(dx);
    int sign_y = sign(dy);

    if (len_y > len_x)
    {
        swap(&len_x, &len_y);
        changed = true;
    }

    // Depth
    fvec3 f0 = {(float)v0.x, (float)v0.y, 0};
    fvec3 f1 = {(float)v1.x, (float)v1.y, 0};
    fvec3 f0f1 = fvec3_sub(f1, f0);
    float fl = fvec3_length(f0f1);

    // Step along line
    int e = 2 * len_y - len_x;
    for (int i = 0; i < len_x; ++i)
    {
        fvec3 p = {(float)x, (float)y, 0};
        fvec3 interp = fvec3_sub(f0, p);
        float interp_len = fvec3_length(interp);
        float alpha = interp_len / fl;

        float depth = v0.z + (v1.z - v0.z) * alpha;

        render_pixel(p_user_data, x, y, depth);

        if (points != NULL)
        {
            points[head] = x;
        }

        if (depths != NULL)
        {
            depths[head] = depth;
        }

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

void bresenham_triangle(void *p_user_data, fvec3 v0, fvec3 v1, fvec3 v2)
{
    user_data_struct *user_data = (user_data_struct *)p_user_data;

    int *points_a = (int *)malloc(resolution.y * sizeof(int));
    int *points_b = (int *)malloc(resolution.y * sizeof(int));

    float *depths_a = (float *)malloc(resolution.y * sizeof(float));
    float *depths_b = (float *)malloc(resolution.y * sizeof(float));

    int point_count_a = bresenham_line(p_user_data, points_a, depths_a, v0, v1);
    int point_count_b = bresenham_line(p_user_data, points_b, depths_b, v0, v2);
    bresenham_line(p_user_data, NULL, NULL, v1, v2);

#ifndef WIREFRAME
    int delta_y = (int)v1.y - (int)v0.y;
    int delta_sign = sign(delta_y);

    for (int y = 0; y < point_count_a; ++y)
    {
        int ax = points_a[y];
        float ad = depths_a[y];
        int bx = points_b[y];
        float bd = depths_b[y];

        int x0;
        int x1;
        float d0;
        float d1;

        if(ax < bx) {
            x0 = ax;
            d0 = ad;
            x1 = bx;
            d1 = bd;
        }
        else if(ax > bx) {
            x0 = bx;
            d0 = bd;
            x1 = ax;
            d1 = ad;
        }
        else {
            continue;
        }

        x0 = MAX(x0, 0);
        x1 = MIN(x1, resolution.x - 1);

        int row_y = delta_sign > 0 ? y + (int)v0.y : (int)v0.y - y;

        float il = 1.0f / ((float)x1 - (float)x0);

        for (int x = x0; x < x1; ++x)
        {
            float alpha = ((float)x - (float)x0) * il;
            float depth = d0 + alpha * (d1 - d0);
            render_pixel(p_user_data, x, row_y, depth);
        }
    }
#endif

    free(points_a);
    free(points_b);
    
    free(depths_a);
    free(depths_b);
}

int vertical_sort(const void *a, const void *b)
{
    fvec3 *va = (fvec3 *)a;
    fvec3 *vb = (fvec3 *)b;
    return va->y > vb->y ? 1 : -1;
}

void bresenham_triangles(void *p_user_data, void *p_triangles, int vertex_count)
{
    fvec3 *triangles = (fvec3 *)p_triangles;

    for (int i = 0; i < vertex_count; i += 3)
    {
        fvec3 v0 = triangles[i];
        fvec3 v1 = triangles[i + 1];
        fvec3 v2 = triangles[i + 2];

        fvec3 t[] = {v0, v1, v2};
        qsort(t, 3, sizeof(fvec3), vertical_sort);

        v0 = t[0];
        v1 = t[1];
        v2 = t[2];

        if ((int)v1.y == (int)v2.y)
        {
            bresenham_triangle(p_user_data, v0, v1, v2);
        }
        else if ((int)v0.y == (int)v1.y)
        {
            bresenham_triangle(p_user_data, v2, v0, v1);
        }
        else
        {
            float alpha = (v1.y - v0.y) / (v2.y - v0.y);
            fvec3 v3 = {
                v0.x + alpha * (v2.x - v0.x),
                v1.y,
                v0.z + alpha * (v2.z - v0.z)
            };
            bresenham_triangle(p_user_data, v0, v1, v3);
            bresenham_triangle(p_user_data, v2, v3, v1);
        }
    }
}

void rasterize_triangles(void *p_user_data, void *p_triangles, int vertex_count)
{
    fvec3 *triangles = (fvec3 *)p_triangles;

    // Triangles to clip space
    mat4 proj_mat = projection_matrix(fov, aspect, 0, 1, flip_aspect);

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
        clipped = clip_polygon(clipped, count, np, -z_near, &count);
        clipped = clip_polygon(clipped, count, fp, z_far, &count);
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
    fvec3 *screen_triangles = malloc(sizeof(fvec3) * clipped_vertex_count);
    for (int i = 0; i < clipped_vertex_count; ++i)
    {
        fvec3 vertex = clipped_triangles[i];
        vertex.y *= -1.0f;

        vertex.x += 1.0f;
        vertex.y += 1.0f;

        vertex.x *= ((float)resolution.x - 0.5f) * 0.5f;
        vertex.y *= ((float)resolution.y - 0.5f) * 0.5f;

        screen_triangles[i] = vertex;
    }

    free(clipped_triangles);

    clear_depth_buffer(p_user_data);
    bresenham_triangles(p_user_data, screen_triangles, clipped_vertex_count);

    free(screen_triangles);
}
