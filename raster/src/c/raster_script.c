#include "raster_script.h"

#include "lib_raster.h"
#include "raster.h"
#include "vector.h"

#include <malloc.h>

void *raster_constructor(godot_object *p_instance, void *p_method_data)
{
    user_data_struct *user_data = api->godot_alloc(sizeof(user_data_struct));
    user_data->depth_buffer = malloc(get_pixel_count() * sizeof(int));
    clear_depth_buffer(user_data);
    return user_data;
}

void raster_destructor(godot_object *p_instance, void *p_method_data, void *p_user_data)
{
    user_data_struct *user_data = (user_data_struct *)p_user_data;
    free(user_data->depth_buffer);
    api->godot_free(user_data);
}

godot_variant raster_get_depth_buffer(godot_object *p_instance, void *p_method_data,
                                      void *p_user_data, int p_num_args, godot_variant **p_args)
{
    user_data_struct *user_data = (user_data_struct *)p_user_data;

    godot_variant depth_buffer_var;
    godot_pool_int_array depth_buffer_array;

    api->godot_pool_int_array_new(&depth_buffer_array);
    api->godot_pool_int_array_resize(&depth_buffer_array, get_pixel_count());

    for (int i = 0; i < get_pixel_count(); ++i)
    {
        api->godot_pool_int_array_set(&depth_buffer_array, i, user_data->depth_buffer[i]);
    }

    api->godot_variant_new_pool_int_array(&depth_buffer_var, &depth_buffer_array);
    api->godot_pool_int_array_destroy(&depth_buffer_array);

    return depth_buffer_var;
}

godot_variant raster_clear_depth_buffer(godot_object *p_instance, void *p_method_data,
                                        void *p_user_data, int p_num_args, godot_variant **p_args)
{
    user_data_struct *user_data = (user_data_struct *)p_user_data;
    clear_depth_buffer(user_data);

    godot_variant null_ret;
    api->godot_variant_new_nil(&null_ret);
    return null_ret;
}

godot_variant raster_bresenham_line(godot_object *p_instance, void *p_method_data,
                                    void *p_user_data, int p_num_args, godot_variant **p_args)
{
    user_data_struct *user_data = (user_data_struct *)p_user_data;

    godot_vector2 gv0 = api->godot_variant_as_vector2(p_args[0]);
    godot_vector2 gv1 = api->godot_variant_as_vector2(p_args[1]);

    ivec2 v0 = ivec2_from_godot_vector2(&gv0);
    ivec2 v1 = ivec2_from_godot_vector2(&gv1);

    bresenham_line(p_user_data, NULL, v0, v1);

    godot_variant null_ret;
    api->godot_variant_new_nil(&null_ret);
    return null_ret;
}

godot_variant raster_bresenham_triangle(godot_object *p_instance, void *p_method_data,
                                        void *p_user_data, int p_num_args, godot_variant **p_args)
{
    user_data_struct *user_data = (user_data_struct *)p_user_data;

    godot_vector2 gv0 = api->godot_variant_as_vector2(p_args[0]);
    godot_vector2 gv1 = api->godot_variant_as_vector2(p_args[1]);
    godot_vector2 gv2 = api->godot_variant_as_vector2(p_args[2]);

    ivec2 v0 = ivec2_from_godot_vector2(&gv0);
    ivec2 v1 = ivec2_from_godot_vector2(&gv1);
    ivec2 v2 = ivec2_from_godot_vector2(&gv2);

    bresenham_triangle(p_user_data, v0, v1, v2);

    godot_variant null_ret;
    api->godot_variant_new_nil(&null_ret);
    return null_ret;
}

godot_variant raster_bresenham_triangles(godot_object *p_instance, void *p_method_data,
                                         void *p_user_data, int p_num_args, godot_variant **p_args)
{
    user_data_struct *user_data = (user_data_struct *)p_user_data;

    godot_pool_vector2_array triangle_pool = api->godot_variant_as_pool_vector2_array(p_args[0]);
    int vertex_count = api->godot_pool_vector2_array_size(&triangle_pool);

    ivec2 *triangles = (ivec2 *)malloc(vertex_count * sizeof(ivec2));

    for (int i = 0; i < vertex_count; ++i)
    {
        godot_vector2 gv = api->godot_pool_vector2_array_get(&triangle_pool, i);
        triangles[i] = ivec2_from_godot_vector2(&gv);
    }

    bresenham_triangles(p_user_data, triangles, vertex_count);

    free(triangles);

    godot_variant null_ret;
    api->godot_variant_new_nil(&null_ret);
    return null_ret;
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

    godot_variant null_ret;
    api->godot_variant_new_nil(&null_ret);
    return null_ret;
}
