#include "vector.h"

#include "math.h"

float fvec3_sqlen(fvec3 v)
{
    return fvec3_dot(v, v);
}

float fvec3_length(fvec3 v)
{
    float sqlen = fvec3_sqlen(v);
    return (float)sqrt(fvec3_sqlen(v));
}

fvec3 fvec3_normalize(fvec3 v)
{
    float len = fvec3_length(v);
    return fvec3_div_float(v, len);
}

float fvec3_dot(fvec3 lhs, fvec3 rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

fvec3 fvec3_cross(fvec3 lhs, fvec3 rhs)
{
    return (fvec3){
        lhs.y * rhs.z - lhs.z * rhs.y,
        lhs.z * rhs.x - lhs.x * rhs.z,
        lhs.x * rhs.y - lhs.y * rhs.x,
    };
}

fvec3 fvec3_add_float(const fvec3 lhs, const float rhs)
{
    fvec3 result = {lhs.x + rhs, lhs.y + rhs, lhs.z + rhs};
    return result;
};

fvec3 fvec3_sub_float(const fvec3 lhs, const float rhs)
{
    fvec3 result = {lhs.x - rhs, lhs.y - rhs, lhs.z - rhs};
    return result;
};

fvec3 fvec3_mul_float(const fvec3 lhs, const float rhs)
{
    fvec3 result = {lhs.x * rhs, lhs.y * rhs, lhs.z * rhs};
    return result;
};

fvec3 fvec3_div_float(const fvec3 lhs, const float rhs)
{
    fvec3 result = {lhs.x / rhs, lhs.y / rhs, lhs.z / rhs};
    return result;
};
