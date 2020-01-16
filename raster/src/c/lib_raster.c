#include "lib_raster.h"

#include "raster_script.h"

#include <malloc.h>

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

    // get_resolution
    godot_instance_method get_resolution = {NULL, NULL, NULL};
    get_resolution.method = &raster_get_resolution;

    nativescript_api->godot_nativescript_register_method(p_handle, "Raster", "get_resolution",
                                                         attributes, get_resolution);

    // get_resolution
    godot_instance_method get_z_far = {NULL, NULL, NULL};
    get_z_far.method = &raster_get_z_far;

    nativescript_api->godot_nativescript_register_method(p_handle, "Raster", "get_z_far",
                                                         attributes, get_z_far);

    // set_fov
    godot_instance_method set_resolution = {NULL, NULL, NULL};
    set_resolution.method = &raster_set_resolution;

    nativescript_api->godot_nativescript_register_method(p_handle, "Raster", "set_resolution",
                                                         attributes, set_resolution);

    // set_fov
    godot_instance_method set_fov = {NULL, NULL, NULL};
    set_fov.method = &raster_set_fov;

    nativescript_api->godot_nativescript_register_method(p_handle, "Raster", "set_fov",
                                                         attributes, set_fov);

    // set_aspect
    godot_instance_method set_aspect = {NULL, NULL, NULL};
    set_aspect.method = &raster_set_aspect;

    nativescript_api->godot_nativescript_register_method(p_handle, "Raster", "set_aspect",
                                                         attributes, set_aspect);

    // set_z_near
    godot_instance_method set_z_near = {NULL, NULL, NULL};
    set_z_near.method = &raster_set_z_near;

    nativescript_api->godot_nativescript_register_method(p_handle, "Raster", "set_z_near",
                                                         attributes, set_z_near);

    // set_z_far
    godot_instance_method set_z_far = {NULL, NULL, NULL};
    set_z_far.method = &raster_set_z_far;

    nativescript_api->godot_nativescript_register_method(p_handle, "Raster", "set_z_far",
                                                         attributes, set_z_far);

    // get_depth
    godot_instance_method get_depth = {NULL, NULL, NULL};
    get_depth.method = &raster_get_depth;

    nativescript_api->godot_nativescript_register_method(p_handle, "Raster", "get_depth",
                                                         attributes, get_depth);

    // get_depth_buffer
    godot_instance_method get_depth_buffer = {NULL, NULL, NULL};
    get_depth_buffer.method = &raster_get_depth_buffer;

    nativescript_api->godot_nativescript_register_method(p_handle, "Raster", "get_depth_buffer",
                                                         attributes, get_depth_buffer);

    // clear_depth_buffer
    godot_instance_method clear_depth_buffer = {NULL, NULL, NULL};
    clear_depth_buffer.method = &raster_clear_depth_buffer;

    nativescript_api->godot_nativescript_register_method(p_handle, "Raster", "clear_depth_buffer",
                                                         attributes, clear_depth_buffer);

    // bresenham_line
    godot_instance_method bresenham_line = {NULL, NULL, NULL};
    bresenham_line.method = &raster_bresenham_line;

    nativescript_api->godot_nativescript_register_method(p_handle, "Raster", "bresenham_line",
                                                         attributes, bresenham_line);

    // bresenham_triangle
    godot_instance_method bresenham_triangle = {NULL, NULL, NULL};
    bresenham_triangle.method = &raster_bresenham_triangle;

    nativescript_api->godot_nativescript_register_method(p_handle, "Raster", "bresenham_triangle",
                                                         attributes, bresenham_triangle);

    // bresenham_triangles
    godot_instance_method bresenham_triangles = {NULL, NULL, NULL};
    bresenham_triangles.method = &raster_bresenham_triangles;

    nativescript_api->godot_nativescript_register_method(p_handle, "Raster", "bresenham_triangles",
                                                         attributes, bresenham_triangles);

    // rasterize_triangles
    godot_instance_method rasterize_triangles = {NULL, NULL, NULL};
    rasterize_triangles.method = &raster_rasterize_triangles;

    nativescript_api->godot_nativescript_register_method(p_handle, "Raster", "rasterize_triangles",
                                                         attributes, rasterize_triangles);

    // rasterize_objects
    godot_instance_method rasterize_objects = {NULL, NULL, NULL};
    rasterize_objects.method = &raster_rasterize_objects;

    nativescript_api->godot_nativescript_register_method(p_handle, "Raster", "rasterize_objects",
                                                         attributes, rasterize_objects);
}
