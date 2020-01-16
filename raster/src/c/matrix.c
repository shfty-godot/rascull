#include "matrix.h"

#include <math.h>

#include "raster_math.h"
#include "vector.h"

mat4 mat4_identity()
{
    mat4 out = {{1, 0, 0, 0,
                 0, 1, 0, 0,
                 0, 0, 1, 0,
                 0, 0, 0, 1}};
    return out;
}

fvec4 mat4_mul_fvec4(mat4 m, fvec4 v)
{
    fvec4 out;
    out.x = m.m[0] * v.x + m.m[4] * v.y + m.m[8] * v.z + m.m[12] * v.w;
    out.y = m.m[1] * v.x + m.m[5] * v.y + m.m[9] * v.z + m.m[13] * v.w;
    out.z = m.m[2] * v.x + m.m[6] * v.y + m.m[10] * v.z + m.m[14] * v.w;
    out.w = m.m[3] * v.x + m.m[7] * v.y + m.m[11] * v.z + m.m[15] * v.w;
    return out;
}

float get_fovy(float fov_x, float aspect)
{
    return (float)(RAD2DEG * atan(aspect * tan(DEG2RAD * fov_x * 0.5)) * 2.0);
}

mat4 projection_matrix(float fovy_degrees, float aspect, float z_near, float z_far, bool flip_fov)
{
    if (flip_fov)
    {
        fovy_degrees = get_fovy(fovy_degrees, 1.0f / aspect);
    }

    float delta_z = z_far - z_near;
    float radians = (float)(fovy_degrees / 2.0 * PI / 180.0);
    float sine = (float)sin(radians);

    if (delta_z == 0 || sine == 0 || aspect == 0)
    {
        return mat4_identity();
    }

    float cotangent = (float)(cos(radians) / sine);

    mat4 out = {
        {
            cotangent / aspect, 0, 0, 0,
            0, cotangent, 0, 0,
            0, 0, -(z_far + z_near) / delta_z, 1,
            0, 0, -2 * z_near * z_far / delta_z, 0
        }
    };
    return out;
}
