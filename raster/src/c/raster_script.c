#include "raster_script.h"

#include "lib_raster.h"
#include "raster.h"
#include "vector.h"
#include "matrix.h"
#include "stdio.h"
#include "vector_simd.h"

#include <malloc.h>

// Utility Macros
#define GD_USER_DATA() \
    user_data_struct *user_data = (user_data_struct *)p_user_data

#define GD_NEW(type, name, ...) \
    godot_##type name;          \
    api->godot_##type##_new(&name, ##__VA_ARGS__)

#define GD_VARIANT_NEW(type, name, ...) \
    godot_variant name;                 \
    api->godot_variant_new_##type(&##name, ##__VA_ARGS__)

#define GD_CAST_VARIANT(type, variant) \
    api->godot_variant_as_##type##(variant)

#define GD_VARIANT_ARGV(type, name, idx) \
    godot_##type name = GD_CAST_VARIANT(type, p_args[##idx])

#define GD_ARRAY_GET(array, idx) \
    api->godot_array_get(array, idx)

#define GD_POOL_ARRAY_GET(type, array, idx) \
    api->godot_pool_##type##_array_get(array, idx)

#define GD_ARRAY_SIZE(array) \
    api->godot_array_size(array)

#define GD_POOL_ARRAY_SIZE(type, array) \
    api->godot_pool_##type##_array_size(array)

#define GD_DESTROY(type, name) \
    api->godot_##type##_destroy(name)

#define GD_RETURN_NULL()           \
    GD_VARIANT_NEW(nil, null_ret); \
    return null_ret

mat4 view_mat;
mat4 view_mat_inv;

// Plugin implementation
void *raster_constructor(godot_object *p_instance, void *p_method_data)
{
    user_data_struct *user_data = api->godot_alloc(sizeof(user_data_struct));
    user_data->depth_buffer = malloc(get_pixel_count() * sizeof(float));
    clear_depth_buffer(user_data);
    return user_data;
}

void raster_destructor(godot_object *p_instance, void *p_method_data, void *p_user_data)
{
    GD_USER_DATA();

    free(user_data->depth_buffer);
    api->godot_free(user_data);
}

godot_variant raster_get_resolution(godot_object *p_instance, void *p_method_data,
                                    void *p_user_data, int p_num_args, godot_variant **p_args)
{
    GD_NEW(vector2, res, (float)resolution.x, (float)resolution.y);
    GD_VARIANT_NEW(vector2, res_var, &res);
    return res_var;
}

godot_variant raster_get_z_near(godot_object *p_instance, void *p_method_data,
                                void *p_user_data, int p_num_args, godot_variant **p_args)
{
    GD_VARIANT_NEW(real, z_near_var, z_near);
    return z_near_var;
}

godot_variant raster_get_z_far(godot_object *p_instance, void *p_method_data,
                               void *p_user_data, int p_num_args, godot_variant **p_args)
{
    GD_VARIANT_NEW(real, z_far_var, z_far);
    return z_far_var;
}

godot_variant raster_set_resolution(godot_object *p_instance, void *p_method_data,
                                    void *p_user_data, int p_num_args, godot_variant **p_args)
{
    int x = (int)GD_CAST_VARIANT(int, p_args[0]);
    int y = (int)GD_CAST_VARIANT(int, p_args[1]);

    set_resolution(p_user_data, x, y);

    GD_RETURN_NULL();
}

godot_variant raster_set_flip_fov(godot_object *p_instance, void *p_method_data,
                                  void *p_user_data, int p_num_args, godot_variant **p_args)
{
    flip_aspect = GD_CAST_VARIANT(bool, p_args[0]);

    GD_RETURN_NULL();
}

godot_variant raster_set_fov(godot_object *p_instance, void *p_method_data,
                             void *p_user_data, int p_num_args, godot_variant **p_args)
{
    fov = (float)GD_CAST_VARIANT(real, p_args[0]);

    GD_RETURN_NULL();
}

godot_variant raster_set_aspect(godot_object *p_instance, void *p_method_data,
                                void *p_user_data, int p_num_args, godot_variant **p_args)
{
    aspect = (float)GD_CAST_VARIANT(real, p_args[0]);

    GD_RETURN_NULL();
}

godot_variant raster_set_z_near(godot_object *p_instance, void *p_method_data,
                                void *p_user_data, int p_num_args, godot_variant **p_args)
{
    z_near = (float)GD_CAST_VARIANT(real, p_args[0]);

    GD_RETURN_NULL();
}

godot_variant raster_set_z_far(godot_object *p_instance, void *p_method_data,
                               void *p_user_data, int p_num_args, godot_variant **p_args)
{
    z_far = (float)GD_CAST_VARIANT(real, p_args[0]);

    GD_RETURN_NULL();
}

godot_variant raster_set_view_matrices(godot_object *p_instance, void *p_method_data,
                                       void *p_user_data, int p_num_args, godot_variant **p_args)
{
    GD_VARIANT_ARGV(pool_real_array, g_view_mat, 0);
    GD_VARIANT_ARGV(pool_real_array, g_view_mat_inv, 1);

    view_mat = mat4_from_godot_pool_real_array(&g_view_mat);
    view_mat_inv = mat4_from_godot_pool_real_array(&g_view_mat_inv);

    GD_DESTROY(pool_real_array, &g_view_mat);
    GD_DESTROY(pool_real_array, &g_view_mat_inv);

    GD_RETURN_NULL();
}

godot_variant raster_get_depth(godot_object *p_instance, void *p_method_data,
                               void *p_user_data, int p_num_args, godot_variant **p_args)
{
    GD_USER_DATA();

    int x = (int)GD_CAST_VARIANT(int, p_args[0]);
    int y = (int)GD_CAST_VARIANT(int, p_args[0]);

    GD_VARIANT_NEW(real, depth_var, user_data->depth_buffer[x + y * resolution.x]);

    return depth_var;
}

godot_variant raster_get_depth_buffer(godot_object *p_instance, void *p_method_data,
                                      void *p_user_data, int p_num_args, godot_variant **p_args)
{
    GD_USER_DATA();

    GD_NEW(pool_real_array, depth_buffer_array);

    api->godot_pool_real_array_resize(&depth_buffer_array, get_pixel_count());

    for (int i = 0; i < get_pixel_count(); ++i)
    {
        api->godot_pool_real_array_set(&depth_buffer_array, i, user_data->depth_buffer[i]);
    }

    GD_VARIANT_NEW(pool_real_array, depth_buffer_var, &depth_buffer_array);
    GD_DESTROY(pool_real_array, &depth_buffer_array);

    return depth_buffer_var;
}

godot_variant raster_clear_depth_buffer(godot_object *p_instance, void *p_method_data,
                                        void *p_user_data, int p_num_args, godot_variant **p_args)
{
    GD_USER_DATA();

    clear_depth_buffer(user_data);

    GD_RETURN_NULL();
}

godot_variant raster_bresenham_line(godot_object *p_instance, void *p_method_data,
                                    void *p_user_data, int p_num_args, godot_variant **p_args)
{
    GD_USER_DATA();
    GD_VARIANT_ARGV(vector3, gv0, 0);
    GD_VARIANT_ARGV(vector3, gv1, 1);

    fvec3 v0 = fvec3_from_godot_vector3(&gv0);
    fvec3 v1 = fvec3_from_godot_vector3(&gv1);

    bresenham_line(p_user_data, NULL, NULL, v0, v1, false, NULL);

    GD_RETURN_NULL();
}

godot_variant raster_bresenham_triangle(godot_object *p_instance, void *p_method_data,
                                        void *p_user_data, int p_num_args, godot_variant **p_args)
{
    GD_USER_DATA();
    GD_VARIANT_ARGV(vector3, gv0, 0);
    GD_VARIANT_ARGV(vector3, gv1, 1);
    GD_VARIANT_ARGV(vector3, gv2, 2);

    fvec3 v0 = fvec3_from_godot_vector3(&gv0);
    fvec3 v1 = fvec3_from_godot_vector3(&gv1);
    fvec3 v2 = fvec3_from_godot_vector3(&gv2);

    bresenham_triangle(p_user_data, v0, v1, v2, false);

    GD_RETURN_NULL();
}

godot_variant raster_bresenham_triangles(godot_object *p_instance, void *p_method_data,
                                         void *p_user_data, int p_num_args, godot_variant **p_args)
{
    GD_USER_DATA();
    GD_VARIANT_ARGV(pool_vector3_array, triangle_pool, 0);

    int vertex_count = GD_POOL_ARRAY_SIZE(vector3, &triangle_pool);
    fvec3 *triangles = (fvec3 *)malloc(vertex_count * sizeof(fvec3));

    for (int i = 0; i < vertex_count; ++i)
    {
        godot_vector3 gv = GD_POOL_ARRAY_GET(vector3, &triangle_pool, i);
        triangles[i] = fvec3_from_godot_vector3(&gv);
    }

    GD_DESTROY(pool_vector3_array, &triangle_pool);

    bresenham_triangles(p_user_data, triangles, vertex_count, false);

    free(triangles);

    GD_RETURN_NULL();
}

godot_variant raster_rasterize_triangles(godot_object *p_instance, void *p_method_data,
                                         void *p_user_data, int p_num_args, godot_variant **p_args)
{
    GD_USER_DATA();
    GD_VARIANT_ARGV(pool_vector3_array, triangle_pool, 0);

    int vertex_count = GD_POOL_ARRAY_SIZE(vector3, &triangle_pool);
    fvec3 *triangles = (fvec3 *)malloc(vertex_count * sizeof(fvec3));

    for (int i = 0; i < vertex_count; ++i)
    {
        godot_vector3 gv = GD_POOL_ARRAY_GET(vector3, &triangle_pool, i);
        triangles[i] = fvec3_from_godot_vector3(&gv);
    }

    GD_DESTROY(pool_vector3_array, &triangle_pool);

    // Rasterize
    verts_to_clip_space(triangles, vertex_count);
    triangles = clip_triangles(triangles, &vertex_count);
    verts_to_ndc(triangles, vertex_count);
    verts_to_screen_space(triangles, vertex_count);
    bresenham_triangles(p_user_data, triangles, vertex_count, false);

    free(triangles);

    GD_RETURN_NULL();
}

fvec3 *prepare_triangles(godot_array *p_matrices_triangles, int *o_vertex_count)
{
    fvec3 *triangles = (fvec3 *)malloc(0);

    int vertex_count = *o_vertex_count;

    // Iterate over object matrices / triangles
    for (int i = 0; i < GD_ARRAY_SIZE(p_matrices_triangles); ++i)
    {
        // Retrieve matrices / triangles array
        godot_variant mo_var = GD_ARRAY_GET(p_matrices_triangles, i);
        godot_array mo = GD_CAST_VARIANT(array, &mo_var);
        GD_DESTROY(variant, &mo_var);

        // Retrieve world matrix
        godot_variant mat_var = GD_ARRAY_GET(&mo, 1);
        godot_pool_real_array g_mat = GD_CAST_VARIANT(pool_real_array, &mat_var);
        GD_DESTROY(variant, &mat_var);

        // Retrieve inverse world matrix
        godot_variant mat_inv_var = GD_ARRAY_GET(&mo, 2);
        godot_pool_real_array g_mat_inv = GD_CAST_VARIANT(pool_real_array, &mat_inv_var);
        GD_DESTROY(variant, &mat_inv_var);

        // Retrieve triangles
        godot_variant tris_var = GD_ARRAY_GET(&mo, 3);
        godot_pool_vector3_array obj_tris = GD_CAST_VARIANT(pool_vector3_array, &tris_var);
        GD_DESTROY(variant, &tris_var);

        GD_DESTROY(array, &mo);

        // Convert world matrix
        mat4 mat;
        for (int mi = 0; mi < GD_POOL_ARRAY_SIZE(real, &g_mat); ++mi)
        {
            float v = GD_POOL_ARRAY_GET(real, &g_mat, mi);
            mat.m[mi] = v;
        }

        GD_DESTROY(pool_real_array, &g_mat);

        // Convert inverse world matrix
        mat4 mat_inv;
        for (int mi = 0; mi < GD_POOL_ARRAY_SIZE(real, &g_mat_inv); ++mi)
        {
            float v = GD_POOL_ARRAY_GET(real, &g_mat_inv, mi);
            mat_inv.m[mi] = v;
        }

        GD_DESTROY(pool_real_array, &g_mat_inv);

        // Convert triangles
        int obj_vert_count = GD_POOL_ARRAY_SIZE(vector3, &obj_tris);

        triangles = realloc(triangles, (vertex_count + obj_vert_count) * sizeof(fvec3));

        for (int i = 0; i < obj_vert_count; i += 3)
        {
            godot_vector3 gv0 = GD_POOL_ARRAY_GET(vector3, &obj_tris, i);
            godot_vector3 gv1 = GD_POOL_ARRAY_GET(vector3, &obj_tris, i + 1);
            godot_vector3 gv2 = GD_POOL_ARRAY_GET(vector3, &obj_tris, i + 2);

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

        GD_DESTROY(pool_vector3_array, &obj_tris);
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
    GD_VARIANT_ARGV(array, matrices_triangles, 0);

    // Prepare triangles
    int vert_count = 0;
    fvec3 *triangles = prepare_triangles(&matrices_triangles, &vert_count);

    GD_DESTROY(array, &matrices_triangles);

    // Rasterize
    bresenham_triangles(p_user_data, triangles, vert_count, false);

    // Cleanup
    free(triangles);

    GD_RETURN_NULL();
}

godot_variant raster_depth_test(godot_object *p_instance, void *p_method_data,
                                void *p_user_data, int p_num_args, godot_variant **p_args)
{
    // Retrieve parameters
    GD_VARIANT_ARGV(array, matrices_triangles, 0);

    // Prepare triangles
    int vert_count = 0;
    fvec3 *triangles = prepare_triangles(&matrices_triangles, &vert_count);

    GD_DESTROY(array, &matrices_triangles);

    // Rasterize
    bool success = bresenham_triangles(p_user_data, triangles, vert_count, true);

    // Cleanup
    free(triangles);

    GD_VARIANT_NEW(bool, ret_var, success);
    return ret_var;
}

godot_variant raster_simd_test(godot_object *p_instance, void *p_method_data,
                               void *p_user_data, int p_num_args, godot_variant **p_args)
{
    svec lhs = svec_new_4(1.0f, 2.0f, 3.0f, 4.0f);
    svec rhs = svec_new_4(5.0f, 6.0f, 7.0f, 8.0f);

    print_svec("lhs", lhs);
    print_svec("rhs", rhs);

    svec add_result = simd_add(lhs, rhs);
    print_svec("simd add", add_result);

    svec sub_result = simd_sub(lhs, rhs);
    print_svec("simd sub", sub_result);

    svec mul_result = simd_mul(lhs, rhs);
    print_svec("simd mul", mul_result);

    svec div_result = simd_div(lhs, rhs);
    print_svec("simd div", div_result);

    svec sqrt_result = simd_sqrt(lhs);
    print_svec("simd sqrt lhs", sqrt_result);

    float dot_result = simd_dot(lhs, rhs);
    printf("simd dot: %f\n", dot_result);

    svec cross_result = simd_cross(lhs, rhs);
    print_svec("simd cross", cross_result);

    GD_RETURN_NULL();
}
