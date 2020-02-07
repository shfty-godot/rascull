#include "raster_script.h"

#include "lib_raster.h"
#include "raster.h"
#include "vector.h"
#include "matrix.h"
#include "stdio.h"

#include <malloc.h>

// Utility Macros
#define USER_DATA() \
    user_data_struct *user_data = (user_data_struct *)p_user_data

#define GODOT_NEW(type, name, ...) \
    godot_##type name;             \
    api->godot_##type##_new(&name, ##__VA_ARGS__)

#define GODOT_VARIANT_NEW(type, name, ...) \
    godot_variant name;                    \
    api->godot_variant_new_##type(&##name, ##__VA_ARGS__)

#define GODOT_CAST_VARIANT(type, variant) \
    api->godot_variant_as_##type##(variant)

#define VARIANT_ARGV(type, name, idx) \
    godot_##type name = GODOT_CAST_VARIANT(type, p_args[##idx])

#define GODOT_DESTROY(type, name) \
    api->godot_##type##_destroy(&name)

#define RETURN_NULL_VARIANT() \
    GODOT_VARIANT_NEW(nil, null_ret); \
    return null_ret

// Plugin implementation
void *
raster_constructor(godot_object *p_instance, void *p_method_data)
{
    user_data_struct *user_data = api->godot_alloc(sizeof(user_data_struct));
    user_data->depth_buffer = malloc(get_pixel_count() * sizeof(float));
    clear_depth_buffer(user_data);
    return user_data;
}

void raster_destructor(godot_object *p_instance, void *p_method_data, void *p_user_data)
{
    USER_DATA();

    free(user_data->depth_buffer);
    api->godot_free(user_data);
}

godot_variant raster_get_resolution(godot_object *p_instance, void *p_method_data,
                                    void *p_user_data, int p_num_args, godot_variant **p_args)
{
    GODOT_NEW(vector2, res, (float)resolution.x, (float)resolution.y);
    GODOT_VARIANT_NEW(vector2, res_var, &res);
    return res_var;
}

godot_variant raster_get_z_near(godot_object *p_instance, void *p_method_data,
                                void *p_user_data, int p_num_args, godot_variant **p_args)
{
    GODOT_VARIANT_NEW(real, z_near_var, z_near);
    return z_near_var;
}

godot_variant raster_get_z_far(godot_object *p_instance, void *p_method_data,
                               void *p_user_data, int p_num_args, godot_variant **p_args)
{
    GODOT_VARIANT_NEW(real, z_far_var, z_far);
    return z_far_var;
}

godot_variant raster_set_resolution(godot_object *p_instance, void *p_method_data,
                                    void *p_user_data, int p_num_args, godot_variant **p_args)
{
    int x = (int)GODOT_CAST_VARIANT(int, p_args[0]);
    int y = (int)GODOT_CAST_VARIANT(int, p_args[1]);

    set_resolution(p_user_data, x, y);

    RETURN_NULL_VARIANT();
}

godot_variant raster_set_flip_fov(godot_object *p_instance, void *p_method_data,
                                  void *p_user_data, int p_num_args, godot_variant **p_args)
{
    flip_aspect = GODOT_CAST_VARIANT(bool, p_args[0]);

    RETURN_NULL_VARIANT();
}

godot_variant raster_set_fov(godot_object *p_instance, void *p_method_data,
                             void *p_user_data, int p_num_args, godot_variant **p_args)
{
    fov = (float)GODOT_CAST_VARIANT(real, p_args[0]);

    RETURN_NULL_VARIANT();
}

godot_variant raster_set_aspect(godot_object *p_instance, void *p_method_data,
                                void *p_user_data, int p_num_args, godot_variant **p_args)
{
    aspect = (float)GODOT_CAST_VARIANT(real, p_args[0]);

    RETURN_NULL_VARIANT();
}

godot_variant raster_set_z_near(godot_object *p_instance, void *p_method_data,
                                void *p_user_data, int p_num_args, godot_variant **p_args)
{
    z_near = (float)GODOT_CAST_VARIANT(real, p_args[0]);

    RETURN_NULL_VARIANT();
}

godot_variant raster_set_z_far(godot_object *p_instance, void *p_method_data,
                               void *p_user_data, int p_num_args, godot_variant **p_args)
{
    z_far = (float)GODOT_CAST_VARIANT(real, p_args[0]);

    RETURN_NULL_VARIANT();
}

godot_variant raster_get_depth(godot_object *p_instance, void *p_method_data,
                               void *p_user_data, int p_num_args, godot_variant **p_args)
{
    USER_DATA();

    int x = (int)GODOT_CAST_VARIANT(int, p_args[0]);
    int y = (int)GODOT_CAST_VARIANT(int, p_args[0]);

    GODOT_VARIANT_NEW(real, depth_var, user_data->depth_buffer[x + y * resolution.x]);

    return depth_var;
}

godot_variant raster_get_depth_buffer(godot_object *p_instance, void *p_method_data,
                                      void *p_user_data, int p_num_args, godot_variant **p_args)
{
    USER_DATA();

    GODOT_NEW(pool_real_array, depth_buffer_array);

    api->godot_pool_real_array_resize(&depth_buffer_array, get_pixel_count());

    for (int i = 0; i < get_pixel_count(); ++i)
    {
        api->godot_pool_real_array_set(&depth_buffer_array, i, user_data->depth_buffer[i]);
    }

    GODOT_VARIANT_NEW(pool_real_array, depth_buffer_var, &depth_buffer_array);
    GODOT_DESTROY(pool_real_array, depth_buffer_array);

    return depth_buffer_var;
}

godot_variant raster_clear_depth_buffer(godot_object *p_instance, void *p_method_data,
                                        void *p_user_data, int p_num_args, godot_variant **p_args)
{
    USER_DATA();

    clear_depth_buffer(user_data);

    RETURN_NULL_VARIANT();
}

godot_variant raster_bresenham_line(godot_object *p_instance, void *p_method_data,
                                    void *p_user_data, int p_num_args, godot_variant **p_args)
{
    USER_DATA();
    VARIANT_ARGV(vector3, gv0, 0);
    VARIANT_ARGV(vector3, gv1, 1);

    fvec3 v0 = fvec3_from_godot_vector3(&gv0);
    fvec3 v1 = fvec3_from_godot_vector3(&gv1);

    bresenham_line(p_user_data, NULL, NULL, v0, v1, false, NULL);

    RETURN_NULL_VARIANT();
}

godot_variant raster_bresenham_triangle(godot_object *p_instance, void *p_method_data,
                                        void *p_user_data, int p_num_args, godot_variant **p_args)
{
    USER_DATA();
    VARIANT_ARGV(vector3, gv0, 0);
    VARIANT_ARGV(vector3, gv1, 1);
    VARIANT_ARGV(vector3, gv2, 2);

    fvec3 v0 = fvec3_from_godot_vector3(&gv0);
    fvec3 v1 = fvec3_from_godot_vector3(&gv1);
    fvec3 v2 = fvec3_from_godot_vector3(&gv2);

    bresenham_triangle(p_user_data, v0, v1, v2, false);

    RETURN_NULL_VARIANT();
}

godot_variant raster_bresenham_triangles(godot_object *p_instance, void *p_method_data,
                                         void *p_user_data, int p_num_args, godot_variant **p_args)
{
    USER_DATA();
    VARIANT_ARGV(pool_vector3_array, triangle_pool, 0);

    int vertex_count = api->godot_pool_vector3_array_size(&triangle_pool);
    fvec3 *triangles = (fvec3 *)malloc(vertex_count * sizeof(fvec3));

    for (int i = 0; i < vertex_count; ++i)
    {
        godot_vector3 gv = api->godot_pool_vector3_array_get(&triangle_pool, i);
        triangles[i] = fvec3_from_godot_vector3(&gv);
    }

    GODOT_DESTROY(pool_vector3_array, triangle_pool);

    bresenham_triangles(p_user_data, triangles, vertex_count, false);

    free(triangles);

    RETURN_NULL_VARIANT();
}

godot_variant raster_rasterize_triangles(godot_object *p_instance, void *p_method_data,
                                         void *p_user_data, int p_num_args, godot_variant **p_args)
{
    USER_DATA();
    VARIANT_ARGV(pool_vector3_array, triangle_pool, 0);

    int vertex_count = api->godot_pool_vector3_array_size(&triangle_pool);
    fvec3 *triangles = (fvec3 *)malloc(vertex_count * sizeof(fvec3));

    for (int i = 0; i < vertex_count; ++i)
    {
        godot_vector3 gv = api->godot_pool_vector3_array_get(&triangle_pool, i);
        triangles[i] = fvec3_from_godot_vector3(&gv);
    }

    GODOT_DESTROY(pool_vector3_array, triangle_pool);

    // Rasterize
    verts_to_clip_space(triangles, vertex_count);
    triangles = clip_triangles(triangles, &vertex_count);
    verts_to_ndc(triangles, vertex_count);
    verts_to_screen_space(triangles, vertex_count);
    bresenham_triangles(p_user_data, triangles, vertex_count, false);

    free(triangles);

    RETURN_NULL_VARIANT();
}

fvec3 *prepare_triangles(godot_array *p_matrices_triangles, mat4 view_mat, mat4 view_mat_inv, int *o_vertex_count)
{
    fvec3 *triangles = (fvec3 *)malloc(0);

    int vertex_count = *o_vertex_count;

    // Iterate over object matrices / triangles
    for (int i = 0; i < api->godot_array_size(p_matrices_triangles); ++i)
    {
        // Retrieve matrices / triangles array
        godot_variant mo_var = api->godot_array_get(p_matrices_triangles, i);
        godot_array mo = GODOT_CAST_VARIANT(array, &mo_var);
        GODOT_DESTROY(variant, mo_var);

        // Retrieve world matrix
        godot_variant mat_var = api->godot_array_get(&mo, 1);
        godot_pool_real_array g_mat = GODOT_CAST_VARIANT(pool_real_array, &mat_var);
        GODOT_DESTROY(variant, mat_var);

        // Retrieve inverse world matrix
        godot_variant mat_inv_var = api->godot_array_get(&mo, 2);
        godot_pool_real_array g_mat_inv = GODOT_CAST_VARIANT(pool_real_array, &mat_inv_var);
        GODOT_DESTROY(variant, mat_inv_var);

        // Retrieve triangles
        godot_variant tris_var = api->godot_array_get(&mo, 3);
        godot_pool_vector3_array obj_tris = GODOT_CAST_VARIANT(pool_vector3_array, &tris_var);
        GODOT_DESTROY(variant, tris_var);

        GODOT_DESTROY(array, mo);

        // Convert world matrix
        mat4 mat;
        for (int mi = 0; mi < api->godot_pool_real_array_size(&g_mat); ++mi)
        {
            float v = api->godot_pool_real_array_get(&g_mat, mi);
            mat.m[mi] = v;
        }

        GODOT_DESTROY(pool_real_array, g_mat);

        // Convert inverse world matrix
        mat4 mat_inv;
        for (int mi = 0; mi < api->godot_pool_real_array_size(&g_mat_inv); ++mi)
        {
            float v = api->godot_pool_real_array_get(&g_mat_inv, mi);
            mat_inv.m[mi] = v;
        }

        GODOT_DESTROY(pool_real_array, g_mat_inv);

        // Convert triangles
        int obj_vert_count = api->godot_pool_vector3_array_size(&obj_tris);

        triangles = realloc(triangles, (vertex_count + obj_vert_count) * sizeof(fvec3));

        for (int i = 0; i < obj_vert_count; i += 3)
        {
            godot_vector3 gv0 = api->godot_pool_vector3_array_get(&obj_tris, i);
            godot_vector3 gv1 = api->godot_pool_vector3_array_get(&obj_tris, i + 1);
            godot_vector3 gv2 = api->godot_pool_vector3_array_get(&obj_tris, i + 2);

            fvec3 v0 = fvec3_from_godot_vector3(&gv0);
            fvec3 v1 = fvec3_from_godot_vector3(&gv1);
            fvec3 v2 = fvec3_from_godot_vector3(&gv2);

            fvec4 vt0 = mat4_mul_fvec4(mat, (fvec4){v0.x, v0.y, v0.z, 1.0f});
            vt0 = mat4_mul_fvec4(view_mat_inv, vt0);

            fvec4 vt1 = mat4_mul_fvec4(mat, (fvec4){v1.x, v1.y, v1.z, 1.0f});
            vt1 = mat4_mul_fvec4(view_mat_inv, vt1);

            fvec4 vt2 = mat4_mul_fvec4(mat, (fvec4){v2.x, v2.y, v2.z, 1.0f});
            vt2 = mat4_mul_fvec4(view_mat_inv, vt2);

            // Cull backfaces
            v0 = (fvec3){vt0.x, vt0.y, vt0.z};
            v1 = (fvec3){vt1.x, vt1.y, vt1.z};
            v2 = (fvec3){vt2.x, vt2.y, vt2.z};

            fvec3 vn = fvec3_normalize(fvec3_cross(fvec3_sub(v2, v0), fvec3_sub(v1, v0)));

            fvec4 local_camera_4 = mat4_mul_fvec4(view_mat, (fvec4){0.0f, 0.0f, 0.0f, 0.0f});
            local_camera_4 = mat4_mul_fvec4(mat_inv, local_camera_4);
            fvec3 local_camera = (fvec3){local_camera_4.z, local_camera_4.y, local_camera_4.z};

            fvec3 view_vector = fvec3_normalize(fvec3_sub(v0, local_camera));

            if (fvec3_dot(vn, view_vector) <= 0.0f)
            {
                triangles[vertex_count++] = v0;
                triangles[vertex_count++] = v1;
                triangles[vertex_count++] = v2;
            }
        }

        GODOT_DESTROY(pool_vector3_array, obj_tris);
    }

    triangles = realloc(triangles, vertex_count * sizeof(fvec3));

    verts_to_clip_space(triangles, vertex_count);
    triangles = clip_triangles(triangles, &vertex_count);
    verts_to_ndc(triangles, vertex_count);
    verts_to_screen_space(triangles, vertex_count);

    *o_vertex_count = vertex_count;

    return triangles;
}

godot_variant raster_rasterize_objects(godot_object *p_instance, void *p_method_data,
                                       void *p_user_data, int p_num_args, godot_variant **p_args)
{
    clear_depth_buffer(p_user_data);

    // Retrieve parameters
    VARIANT_ARGV(array, matrices_triangles, 0);
    VARIANT_ARGV(pool_real_array, g_view_mat, 1);
    VARIANT_ARGV(pool_real_array, g_view_mat_inv, 2);

    mat4 view_mat = mat4_from_godot_pool_real_array(&g_view_mat);
    mat4 view_mat_inv = mat4_from_godot_pool_real_array(&g_view_mat_inv);

    GODOT_DESTROY(pool_real_array, g_view_mat);
    GODOT_DESTROY(pool_real_array, g_view_mat_inv);

    // Prepare triangles
    int vert_count = 0;
    fvec3 *triangles = prepare_triangles(&matrices_triangles, view_mat, view_mat_inv, &vert_count);

    GODOT_DESTROY(array, matrices_triangles);

    // Rasterize
    bresenham_triangles(p_user_data, triangles, vert_count, false);

    // Cleanup
    free(triangles);

    RETURN_NULL_VARIANT();
}

godot_variant raster_depth_test(godot_object *p_instance, void *p_method_data,
                                void *p_user_data, int p_num_args, godot_variant **p_args)
{
    // Retrieve parameters
    VARIANT_ARGV(array, matrices_triangles, 0);
    VARIANT_ARGV(pool_real_array, g_view_mat, 1);
    VARIANT_ARGV(pool_real_array, g_view_mat_inv, 2);

    mat4 view_mat = mat4_from_godot_pool_real_array(&g_view_mat);
    mat4 view_mat_inv = mat4_from_godot_pool_real_array(&g_view_mat_inv);

    GODOT_DESTROY(pool_real_array, g_view_mat);
    GODOT_DESTROY(pool_real_array, g_view_mat_inv);

    // Prepare triangles
    int vert_count = 0;
    fvec3 *triangles = prepare_triangles(&matrices_triangles, view_mat, view_mat_inv, &vert_count);

    GODOT_DESTROY(array, matrices_triangles);

    // Rasterize
    bool success = bresenham_triangles(p_user_data, triangles, vert_count, true);

    // Cleanup
    free(triangles);

    GODOT_VARIANT_NEW(bool, ret_var, success);
    return ret_var;
}
