#ifndef VECTOR_H
#define VECTOR_H

#include <gdnative_api_struct.gen.h>

#include "lib_raster.h"

#define VEC2_OP(tn, on, op)                                    \
    inline tn tn##_##on(const tn lhs, const tn rhs)            \
    {                                                          \
        tn result = {lhs.x op rhs.x, lhs.y op rhs.y};          \
        return result;                                         \
    };                                                         \
    inline tn tn##_##on##_float(const tn lhs, const float rhs) \
    {                                                          \
        tn result = {lhs.x op rhs, lhs.y op rhs};              \
        return result;                                         \
    };

#define VEC2_FROM_GVEC2(tn, t)                                                          \
    inline tn tn##_from_godot_vector2(const godot_vector2 *gv)                          \
    {                                                                                   \
        tn result = {(t)api->godot_vector2_get_x(gv), (t)api->godot_vector2_get_y(gv)}; \
        return result;                                                                  \
    }

#define VEC2(n, t)     \
    typedef struct n   \
    {                  \
        t x;           \
        t y;           \
    } n;               \
                       \
    VEC2_OP(n, add, +) \
    VEC2_OP(n, sub, -) \
    VEC2_OP(n, mul, *) \
    VEC2_OP(n, div, /) \
    VEC2_FROM_GVEC2(n, t)

#define VEC3_OP(tn, on, op)                                           \
    inline tn tn##_##on(const tn lhs, const tn rhs)                   \
    {                                                                 \
        tn result = {lhs.x op rhs.x, lhs.y op rhs.y, lhs.z op rhs.z}; \
        return result;                                                \
    };                                                                \
    inline tn tn##_##on##_float(const tn lhs, const float rhs)        \
    {                                                                 \
        tn result = {lhs.x op rhs, lhs.y op rhs, lhs.z op rhs};       \
        return result;                                                \
    };

#define VEC3_FROM_GVEC3(tn, t)                                        \
    inline tn tn##_from_godot_vector3(const godot_vector3 *gv)        \
    {                                                                 \
        tn result = {                                                 \
            (t)api->godot_vector3_get_axis(gv, GODOT_VECTOR3_AXIS_X), \
            (t)api->godot_vector3_get_axis(gv, GODOT_VECTOR3_AXIS_Y), \
            (t)api->godot_vector3_get_axis(gv, GODOT_VECTOR3_AXIS_Z), \
        };                                                            \
        return result;                                                \
    }

#define VEC3(n, t)     \
    typedef struct n   \
    {                  \
        t x;           \
        t y;           \
        t z;           \
    } n;               \
                       \
    VEC3_OP(n, add, +) \
    VEC3_OP(n, sub, -) \
    VEC3_OP(n, mul, *) \
    VEC3_OP(n, div, /) \
    VEC3_FROM_GVEC3(n, t)

#define VEC4_OP(tn, on, op)                                                           \
    inline tn tn##_##on(const tn lhs, const tn rhs)                                   \
    {                                                                                 \
        tn result = {lhs.x op rhs.x, lhs.y op rhs.y, lhs.z op rhs.z, lhs.w op rhs.w}; \
        return result;                                                                \
    };                                                                                \
    inline tn tn##_##on##_float(const tn lhs, const float rhs)                        \
    {                                                                                 \
        tn result = {lhs.x op rhs, lhs.y op rhs, lhs.z op rhs, lhs.w op rhs};         \
        return result;                                                                \
    };

#define VEC4(n, t)     \
    typedef struct n   \
    {                  \
        t x;           \
        t y;           \
        t z;           \
        t w;           \
    } n;               \
                       \
    VEC4_OP(n, add, +) \
    VEC4_OP(n, sub, -) \
    VEC4_OP(n, mul, *) \
    VEC4_OP(n, div, /)

VEC2(ivec2, int)
VEC3(ivec3, int)
VEC4(ivec4, int)

VEC2(fvec2, float)
VEC3(fvec3, float)
VEC4(fvec4, float)

float fvec3_dot(fvec3 lhs, fvec3 rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

#endif