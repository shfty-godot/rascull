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
    nativescript_api->godot_nativescript_register_class(p_handle, "Simple", "Node",
                                                        create, destroy);

    // Register methods
    godot_method_attributes attributes = {GODOT_METHOD_RPC_MODE_DISABLED};

    // get_depth_buffer
    godot_instance_method get_depth_buffer = {NULL, NULL, NULL};
    get_depth_buffer.method = &raster_get_depth_buffer;

    nativescript_api->godot_nativescript_register_method(p_handle, "Simple", "get_depth_buffer",
                                                         attributes, get_depth_buffer);

    // clear_depth_buffer
    godot_instance_method clear_depth_buffer = {NULL, NULL, NULL};
    clear_depth_buffer.method = &raster_clear_depth_buffer;

    nativescript_api->godot_nativescript_register_method(p_handle, "Simple", "clear_depth_buffer",
                                                         attributes, clear_depth_buffer);

    // bresenham_line
    godot_instance_method bresenham_line = {NULL, NULL, NULL};
    bresenham_line.method = &raster_bresenham_line;

    nativescript_api->godot_nativescript_register_method(p_handle, "Simple", "bresenham_line",
                                                         attributes, bresenham_line);

    // bresenham_triangle
    godot_instance_method bresenham_triangle = {NULL, NULL, NULL};
    bresenham_triangle.method = &raster_bresenham_triangle;

    nativescript_api->godot_nativescript_register_method(p_handle, "Simple", "bresenham_triangle",
                                                         attributes, bresenham_triangle);

    // bresenham_triangles
    godot_instance_method bresenham_triangles = {NULL, NULL, NULL};
    bresenham_triangles.method = &raster_bresenham_triangles;

    nativescript_api->godot_nativescript_register_method(p_handle, "Simple", "bresenham_triangles",
                                                         attributes, bresenham_triangles);

    // rasterize_triangles
    godot_instance_method rasterize_triangles = {NULL, NULL, NULL};
    rasterize_triangles.method = &raster_rasterize_triangles;

    nativescript_api->godot_nativescript_register_method(p_handle, "Simple", "rasterize_triangles",
                                                         attributes, rasterize_triangles);
}
