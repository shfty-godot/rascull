#ifndef VEC2_H
#define VEC2_H

typedef struct vec2
{
    int x;
    int y;
} vec2;

inline vec2 vec2_from_godot_vector2(const godot_vector2* gv) {
    vec2 result = {(int)api->godot_vector2_get_x(gv), (int)api->godot_vector2_get_y(gv)};
    return result;
}

inline vec2 vec2_add(const vec2 lhs, const vec2 rhs) {
    vec2 result = {lhs.x + rhs.x, lhs.y + rhs.y};
    return result;
}

inline vec2 vec2_sub(const vec2 lhs, const vec2 rhs) {
    vec2 result = {lhs.x - rhs.x, lhs.y - rhs.y};
    return result;
}

inline vec2 vec2_mul(const vec2 lhs, const vec2 rhs) {
    vec2 result = {lhs.x * rhs.x, lhs.y * rhs.y};
    return result;
}

inline vec2 vec2_div(const vec2 lhs, const vec2 rhs) {
    vec2 result = {lhs.x / rhs.x, lhs.y / rhs.y};
    return result;
}

#endif