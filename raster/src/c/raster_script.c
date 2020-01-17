#include "raster_script.h"

#include "lib_raster.h"
#include "raster.h"
#include "vector.h"
#include "matrix.h"
#include "stdio.h"

#include <malloc.h>

#define RETURN_NULL_VARIANT()              \
    godot_variant null_ret;                \
    api->godot_variant_new_nil(&null_ret); \
    return null_ret;

void *raster_constructor(godot_object *p_instance, void *p_method_data)
{
    user_data_struct *user_data = api->godot_alloc(sizeof(user_data_struct));
    user_data->depth_buffer = malloc(get_pixel_count() * sizeof(float));
    clear_depth_buffer(user_data);
    return user_data;
}

void raster_destructor(godot_object *p_instance, void *p_method_data, void *p_user_data)
{
    user_data_struct *user_data = (user_data_struct *)p_user_data;
    free(user_data->depth_buffer);
    api->godot_free(user_data);
}

godot_variant raster_get_resolution(godot_object *p_instance, void *p_method_data,
                                    void *p_user_data, int p_num_args, godot_variant **p_args)
{
    godot_vector2 res;
    godot_variant res_var;

    api->godot_vector2_new(&res, (float)resolution.x, (float)resolution.y);
    api->godot_variant_new_vector2(&res_var, &res);

    return res_var;
}

godot_variant raster_get_z_far(godot_object *p_instance, void *p_method_data,
                               void *p_user_data, int p_num_args, godot_variant **p_args)
{
    godot_variant z_far_var;

    api->godot_variant_new_real(&z_far_var, z_far);

    return z_far_var;
}

godot_variant raster_set_resolution(godot_object *p_instance, void *p_method_data,
                                    void *p_user_data, int p_num_args, godot_variant **p_args)
{
    int x = (int)api->godot_variant_as_int(p_args[0]);
    int y = (int)api->godot_variant_as_int(p_args[1]);

    set_resolution(p_user_data, x, y);

    RETURN_NULL_VARIANT();
}

godot_variant raster_set_flip_fov(godot_object *p_instance, void *p_method_data,
                             void *p_user_data, int p_num_args, godot_variant **p_args)
{
    flip_aspect = api->godot_variant_as_bool(p_args[0]);

    RETURN_NULL_VARIANT();
}

godot_variant raster_set_fov(godot_object *p_instance, void *p_method_data,
                             void *p_user_data, int p_num_args, godot_variant **p_args)
{
    fov = (float)api->godot_variant_as_real(p_args[0]);

    RETURN_NULL_VARIANT();
}

godot_variant raster_set_aspect(godot_object *p_instance, void *p_method_data,
                                void *p_user_data, int p_num_args, godot_variant **p_args)
{
    aspect = (float)api->godot_variant_as_real(p_args[0]);

    RETURN_NULL_VARIANT();
}

godot_variant raster_set_z_near(godot_object *p_instance, void *p_method_data,
                                void *p_user_data, int p_num_args, godot_variant **p_args)
{
    z_near = (float)api->godot_variant_as_real(p_args[0]);

    RETURN_NULL_VARIANT();
}

godot_variant raster_set_z_far(godot_object *p_instance, void *p_method_data,
                               void *p_user_data, int p_num_args, godot_variant **p_args)
{
    z_far = (float)api->godot_variant_as_real(p_args[0]);

    RETURN_NULL_VARIANT();
}

godot_variant raster_get_depth(godot_object *p_instance, void *p_method_data,
                               void *p_user_data, int p_num_args, godot_variant **p_args)
{
    user_data_struct *user_data = (user_data_struct *)p_user_data;

    godot_variant depth_var;

    int x = (int)api->godot_variant_as_int(p_args[0]);
    int y = (int)api->godot_variant_as_int(p_args[1]);

    api->godot_variant_new_real(&depth_var, user_data->depth_buffer[x + y * resolution.x]);

    return depth_var;
}

godot_variant raster_get_depth_buffer(godot_object *p_instance, void *p_method_data,
                                      void *p_user_data, int p_num_args, godot_variant **p_args)
{
    user_data_struct *user_data = (user_data_struct *)p_user_data;

    godot_variant depth_buffer_var;
    godot_pool_real_array depth_buffer_array;

    api->godot_pool_real_array_new(&depth_buffer_array);
    api->godot_pool_real_array_resize(&depth_buffer_array, get_pixel_count());

    for (int i = 0; i < get_pixel_count(); ++i)
    {
        api->godot_pool_real_array_set(&depth_buffer_array, i, user_data->depth_buffer[i]);
    }

    api->godot_variant_new_pool_real_array(&depth_buffer_var, &depth_buffer_array);
    api->godot_pool_real_array_destroy(&depth_buffer_array);

    return depth_buffer_var;
}

godot_variant raster_clear_depth_buffer(godot_object *p_instance, void *p_method_data,
                                        void *p_user_data, int p_num_args, godot_variant **p_args)
{
    user_data_struct *user_data = (user_data_struct *)p_user_data;
    clear_depth_buffer(user_data);

    RETURN_NULL_VARIANT();
}

godot_variant raster_bresenham_line(godot_object *p_instance, void *p_method_data,
                                    void *p_user_data, int p_num_args, godot_variant **p_args)
{
    user_data_struct *user_data = (user_data_struct *)p_user_data;

    godot_vector3 gv0 = api->godot_variant_as_vector3(p_args[0]);
    godot_vector3 gv1 = api->godot_variant_as_vector3(p_args[1]);

    fvec3 v0 = fvec3_from_godot_vector3(&gv0);
    fvec3 v1 = fvec3_from_godot_vector3(&gv1);

    bresenham_line(p_user_data, NULL, NULL, v0, v1);

    RETURN_NULL_VARIANT();
}

godot_variant raster_bresenham_triangle(godot_object *p_instance, void *p_method_data,
                                        void *p_user_data, int p_num_args, godot_variant **p_args)
{
    user_data_struct *user_data = (user_data_struct *)p_user_data;

    godot_vector3 gv0 = api->godot_variant_as_vector3(p_args[0]);
    godot_vector3 gv1 = api->godot_variant_as_vector3(p_args[1]);
    godot_vector3 gv2 = api->godot_variant_as_vector3(p_args[2]);

    fvec3 v0 = fvec3_from_godot_vector3(&gv0);
    fvec3 v1 = fvec3_from_godot_vector3(&gv1);
    fvec3 v2 = fvec3_from_godot_vector3(&gv2);

    bresenham_triangle(p_user_data, v0, v1, v2);

    RETURN_NULL_VARIANT();
}

godot_variant raster_bresenham_triangles(godot_object *p_instance, void *p_method_data,
                                         void *p_user_data, int p_num_args, godot_variant **p_args)
{
    user_data_struct *user_data = (user_data_struct *)p_user_data;

    godot_pool_vector3_array triangle_pool = api->godot_variant_as_pool_vector3_array(p_args[0]);
    int vertex_count = api->godot_pool_vector3_array_size(&triangle_pool);

    fvec3 *triangles = (fvec3 *)malloc(vertex_count * sizeof(fvec3));

    for (int i = 0; i < vertex_count; ++i)
    {
        godot_vector3 gv = api->godot_pool_vector3_array_get(&triangle_pool, i);
        triangles[i] = fvec3_from_godot_vector3(&gv);
    }

    bresenham_triangles(p_user_data, triangles, vertex_count);

    free(triangles);

    RETURN_NULL_VARIANT();
}

godot_variant raster_rasterize_triangles(godot_object *p_instance, void *p_method_data,
                                         void *p_user_data, int p_num_args, godot_variant **p_args)
{
    user_data_struct *user_data = (user_data_struct *)p_user_data;

    godot_pool_vector3_array triangle_pool = api->godot_variant_as_pool_vector3_array(p_args[0]);
    int vertex_count = api->godot_pool_vector3_array_size(&triangle_pool);

    fvec3 *triangles = (fvec3 *)malloc(vertex_count * sizeof(fvec3));

    for (int i = 0; i < vertex_count; ++i)
    {
        godot_vector3 gv = api->godot_pool_vector3_array_get(&triangle_pool, i);
        triangles[i] = fvec3_from_godot_vector3(&gv);
    }

    rasterize_triangles(p_user_data, triangles, vertex_count);

    free(triangles);

    RETURN_NULL_VARIANT();
}

godot_variant raster_rasterize_objects(godot_object *p_instance, void *p_method_data,
                                       void *p_user_data, int p_num_args, godot_variant **p_args)
{
    godot_array matrices_triangles = api->godot_variant_as_array(p_args[0]);

    fvec3 *triangles = (fvec3 *)malloc(0);
    int vert_count = 0;

    godot_pool_real_array g_view_mat = api->godot_variant_as_pool_real_array(p_args[1]);

    mat4 view_mat;
    for (int mi = 0; mi < api->godot_pool_real_array_size(&g_view_mat); ++mi)
    {
        float v = api->godot_pool_real_array_get(&g_view_mat, mi);
        view_mat.m[mi] = v;
    }

    for (int i = 0; i < api->godot_array_size(&matrices_triangles); ++i)
    {
        godot_variant mo_var = api->godot_array_get(&matrices_triangles, i);
        godot_array mo = api->godot_variant_as_array(&mo_var);

        godot_variant mat_var = api->godot_array_get(&mo, 0);
        godot_pool_real_array g_mat = api->godot_variant_as_pool_real_array(&mat_var);

        mat4 mat;
        for (int mi = 0; mi < api->godot_pool_real_array_size(&g_mat); ++mi)
        {
            float v = api->godot_pool_real_array_get(&g_mat, mi);
            mat.m[mi] = v;
        }

        godot_variant tris_var = api->godot_array_get(&mo, 1);
        godot_pool_vector3_array obj_tris = api->godot_variant_as_pool_vector3_array(&tris_var);
        int obj_vert_count = api->godot_pool_vector3_array_size(&obj_tris);

        triangles = realloc(triangles, (vert_count + obj_vert_count) * sizeof(fvec3));

        for (int i = 0; i < obj_vert_count; ++i)
        {
            godot_vector3 gv = api->godot_pool_vector3_array_get(&obj_tris, i);
            fvec3 v = fvec3_from_godot_vector3(&gv);
            
            fvec4 vt = mat4_mul_fvec4(mat, (fvec4){v.x, v.y, v.z, 1.0f});
            vt = mat4_mul_fvec4(view_mat, vt);
            triangles[vert_count++] = (fvec3){vt.x, vt.y, vt.z};
        }
    }

    triangles = realloc(triangles, vert_count * sizeof(fvec3));

    rasterize_triangles(p_user_data, triangles, vert_count);

    free(triangles);

    RETURN_NULL_VARIANT();
}
