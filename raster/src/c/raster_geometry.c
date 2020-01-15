#include "raster_geometry.h"

#include "vector.h"
#include "malloc.h"
#include "stdio.h"

#define CMP_POINT_IN_PLANE_EPSILON 0.00001


float plane_distance_to(const fvec3 plane_normal, const float plane_dist, const fvec3 point) {
	return fvec3_dot(plane_normal, point) - plane_dist;
}

fvec3* clip_polygon(const fvec3* p_polygon, const int vertex_count, const fvec3 plane_normal, const float plane_dist, int* o_count)
{
    fvec3 *polygon = (fvec3 *)p_polygon;
    fvec3 *clipped = (fvec3*)malloc(sizeof(fvec3) * vertex_count);

    enum LocationCache
    {
        LOC_INSIDE = 1,
        LOC_BOUNDARY = 0,
        LOC_OUTSIDE = -1
    };

    if (vertex_count == 0)
    {
        *o_count = 0;
        free(polygon);
        return clipped;
    }

    int *location_cache = (int *)malloc(sizeof(int) * vertex_count);
    int inside_count = 0;
    int outside_count = 0;

    for (int a = 0; a < vertex_count; a++)
    {
        float dist = plane_distance_to(plane_normal, plane_dist, polygon[a]);
        if (dist < -CMP_POINT_IN_PLANE_EPSILON)
        {
            location_cache[a] = LOC_INSIDE;
            inside_count++;
        }
        else
        {
            if (dist > CMP_POINT_IN_PLANE_EPSILON)
            {
                location_cache[a] = LOC_OUTSIDE;
                outside_count++;
            }
            else
            {
                location_cache[a] = LOC_BOUNDARY;
            }
        }
    }

    if (outside_count == 0)
    {
        *o_count = vertex_count;
        memcpy(clipped, polygon, sizeof(fvec3) * vertex_count);
        free(polygon);
        free(location_cache);
        return clipped; // No changes.
    }

    else if (inside_count == 0)
    {
        *o_count = 0;
        free(polygon);
        free(location_cache);
        return clipped; // Empty.
    }

    long previous = vertex_count - 1;
    int clipped_count = 0;

    for (int index = 0; index < vertex_count; index++)
    {
        int loc = location_cache[index];
        if (loc == LOC_OUTSIDE)
        {
            if (location_cache[previous] == LOC_INSIDE)
            {
                const fvec3 v1 = polygon[previous];
                const fvec3 v2 = polygon[index];

                fvec3 segment = fvec3_sub(v1, v2);
                float den = fvec3_dot(plane_normal, segment);
                float dist = plane_distance_to(plane_normal, plane_dist, v1) / den;
                dist = -dist;
                clipped_count += 1;
                clipped = realloc(clipped, sizeof(fvec3) * clipped_count);
                clipped[clipped_count - 1] = fvec3_add(v1, fvec3_mul_float(segment, dist));
            }
        }
        else
        {
            const fvec3 v1 = polygon[index];
            if ((loc == LOC_INSIDE) && (location_cache[previous] == LOC_OUTSIDE))
            {
                const fvec3 v2 = polygon[previous];
                fvec3 segment = fvec3_sub(v1, v2);
                float den = fvec3_dot(plane_normal, segment);
                float dist = plane_distance_to(plane_normal, plane_dist, v1) / den;
                dist = -dist;
                clipped_count += 1;
                clipped = realloc(clipped, sizeof(fvec3) * clipped_count);
                clipped[clipped_count - 1] = fvec3_add(v1, fvec3_mul_float(segment, dist));
            }

            clipped_count += 1;
            clipped = (fvec3*)realloc(clipped, sizeof(fvec3) * clipped_count);
            clipped[clipped_count - 1] = v1;
        }

        previous = index;
    }

    *o_count = clipped_count;
    free(polygon);
    free(location_cache);
    return clipped;
}
