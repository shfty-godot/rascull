#ifndef PRINT_H
#define PRINT_H

#include <gdnative_api_struct.gen.h>

#include "lib_raster.h"

void print(const char* p_utf8)
{
    godot_string ptr_str = api->godot_string_chars_to_utf8(p_utf8);
    api->godot_print(&ptr_str);
}

#endif