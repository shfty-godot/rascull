#ifndef RASTER_GEOMETRY_H
#define RASTER_GEOMETRY_H

typedef struct fvec3 fvec3;

float plane_distance_to(const fvec3 plane_normal, const float plane_dist, const fvec3 point);
fvec3* clip_polygon(const fvec3 *p_polygon, const int vertices, const fvec3 plane_normal, const float plane_dist, int* o_count);

#endif
