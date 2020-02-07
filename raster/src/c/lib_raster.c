#include "lib_raster.h"

#include "raster_script.h"

#include <malloc.h>

#define REGISTER_METHOD(name, function)                                                      \
    godot_instance_method name = {NULL, NULL, NULL};                                         \
    name##.method = &function;                                                               \
                                                                                             \
    nativescript_api->godot_nativescript_register_method(p_handle, "Raster", #name, \
                                                         attributes, name);

void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *p_options)
{
    api = p_options->api_struct;

    // Now find our extensions.
    for (unsigned int i = 0; i < api->num_extensions; i++)
    {
        switch (api->extensions[i]->type)
        {
        case GDNATIVE_EXT_NATIVESCRIPT:
        {
            nativescript_api = (godot_gdnative_ext_nativescript_api_struct *)api->extensions[i];
        };
        break;
        default:
            break;
        }
    }
}

void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *p_options)
{
    api = NULL;
    nativescript_api = NULL;
}

void GDN_EXPORT godot_nativescript_init(void *p_handle)
{
    // Constructor
    godot_instance_create_func create = {NULL, NULL, NULL};
    create.create_func = &raster_constructor;

    // Destructor
    godot_instance_destroy_func destroy = {NULL, NULL, NULL};
    destroy.destroy_func = &raster_destructor;

    // Register class
    nativescript_api->godot_nativescript_register_class(p_handle, "Raster", "Node",
                                                        create, destroy);

    // Register methods
    godot_method_attributes attributes = {GODOT_METHOD_RPC_MODE_DISABLED};

    REGISTER_METHOD(get_resolution, raster_get_resolution)
    REGISTER_METHOD(get_z_near, raster_get_z_near)
    REGISTER_METHOD(get_z_far, raster_get_z_far)
    REGISTER_METHOD(set_flip_fov, raster_set_flip_fov)
    REGISTER_METHOD(set_resolution, raster_set_resolution)
    REGISTER_METHOD(set_fov, raster_set_fov)
    REGISTER_METHOD(set_aspect, raster_set_aspect)
    REGISTER_METHOD(set_z_near, raster_set_z_near)
    REGISTER_METHOD(set_z_far, raster_set_z_far)
    REGISTER_METHOD(set_view_matrices, raster_set_view_matrices)
    REGISTER_METHOD(get_depth, raster_get_depth)
    REGISTER_METHOD(get_depth_buffer, raster_get_depth_buffer)
    REGISTER_METHOD(clear_depth_buffer, raster_clear_depth_buffer)
    REGISTER_METHOD(bresenham_line, raster_bresenham_line)
    REGISTER_METHOD(bresenham_triangle, raster_bresenham_triangle)
    REGISTER_METHOD(bresenham_triangles, raster_bresenham_triangles)
    REGISTER_METHOD(rasterize_triangles, raster_rasterize_triangles)
    REGISTER_METHOD(rasterize_objects, raster_rasterize_objects)
    REGISTER_METHOD(depth_test, raster_depth_test)
    REGISTER_METHOD(simd_test, raster_simd_test)
}
