#ifndef RASTER_GEOMETRY
#define RASTER_GEOMETRY

#include "vector.h"
#include "malloc.h"

#define CMP_POINT_IN_PLANE_EPSILON 0.00001


float plane_distance_to(const fvec3 plane_normal, const float plane_dist, const fvec3 point) {
	return fvec3_dot(plane_normal, point) - plane_dist;
}

static inline fvec3* clip_polygon(const fvec3* p_polygon, const int vertices, const fvec3 plane_normal, const float plane_dist)
{
    fvec3 *polygon = (fvec3 *)p_polygon;

    enum LocationCache
    {
        LOC_INSIDE = 1,
        LOC_BOUNDARY = 0,
        LOC_OUTSIDE = -1
    };

    if (vertices == 0)
        return polygon;

    int *location_cache = (int *)malloc(sizeof(int) * vertices);
    int inside_count = 0;
    int outside_count = 0;

    for (int a = 0; a < vertices; a++)
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
        return polygon; // No changes.
    }
    
    else if (inside_count == 0)
    {

        return NULL; // Empty.
    }

    long previous = vertices - 1;
    fvec3* clipped; // TODO: Figure out allocation
    int clipped_count = 0;

    for (int index = 0; index < vertices; index++)
    {
        int loc = location_cache[index];
        if (loc == LOC_OUTSIDE)
        {
            if (location_cache[previous] == LOC_INSIDE)
            {
                const fvec3 &v1 = polygon[previous];
                const fvec3 &v2 = polygon[index];

                fvec3 segment = fvec3_sub(v1, v2);
                float den = fvec3_dot(plane_normal, segment);
                float dist = plane_distance_to(plane_normal, plane_dist, v1) / den;
                dist = -dist;
                clipped[clipped_count++] = fvec3_add(v1, fvec3_mul_float(segment, dist));
            }
        }
        else
        {
            const fvec3 &v1 = polygon[index];
            if ((loc == LOC_INSIDE) && (location_cache[previous] == LOC_OUTSIDE))
            {
                const fvec3 &v2 = polygon[previous];
                fvec3 segment = fvec3_sub(v1, v2);
                float den = fvec3_dot(plane_normal, segment);
                float dist = plane_distance_to(plane_normal, plane_dist, v1) / den;
                dist = -dist;
                clipped[clipped_count++] = fvec3_add(v1, fvec3_mul_float(segment, dist));
            }

            clipped[clipped_count++] = v1;
        }

        previous = index;
    }

    return clipped;
}

#endif