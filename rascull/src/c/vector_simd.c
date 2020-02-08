#include <vector_simd.h>

void print_svec(const char* prefix, svec vec)
{
    float vals[4];
    svec_get(vec, vals);
    printf("%s: %f, %f, %f, %f\n", prefix, vals[0], vals[1], vals[2], vals[3]);
}