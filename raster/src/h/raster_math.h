#ifndef RASTER_MATH_H
#define RASTER_MATH_H

#define PI 3.14159265358979323846

#define RAD2DEG (1.0 * 180.0 / PI)
#define DEG2RAD (PI / 180.0)

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

int sign(const int a);
void swap(int *a, int *b);

#endif