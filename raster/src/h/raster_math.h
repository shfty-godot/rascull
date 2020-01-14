#ifndef RASTER_MATH_H
#define RASTER_MATH_H

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

int sign(const int a);
void swap(int *a, int *b);

#endif