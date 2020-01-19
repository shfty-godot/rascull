#ifndef MATRIX_H
#define MATRIX_H

#include <gdnative_api_struct.gen.h>

typedef struct fvec4 fvec4;

typedef struct mat4
{
    float m[16];
} mat4;

mat4 mat4_identity();
fvec4 mat4_mul_fvec4(mat4 m, fvec4 v);
mat4 mat4_from_godot_pool_real_array(godot_pool_real_array *array);
float get_fovy(float fov_x, float aspect);
mat4 projection_matrix(float fovy_degrees, float aspect, float z_near, float z_far, bool flip_fov);

#endif
