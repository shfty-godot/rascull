#ifndef LIBRASTER_H
#define LIBRASTER_H

#include <gdnative_api_struct.gen.h>

typedef struct user_data_struct
{
    float *depth_buffer;
} user_data_struct;

const godot_gdnative_core_api_struct *api;
const godot_gdnative_ext_nativescript_api_struct *nativescript_api;

#endif
