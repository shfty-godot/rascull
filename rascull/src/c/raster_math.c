#include "raster_math.h"

int sign(const int a)
{
    return (0 < a) - (a < 0);
}

void swap(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}
