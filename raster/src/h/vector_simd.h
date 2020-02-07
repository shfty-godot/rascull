#ifndef VECTOR_SIMD_H
#define VECTOR_SIMD_H

#include <immintrin.h>
#include "stdio.h"

typedef __m128 svec;

inline svec svec_new_4(float x, float y, float z, float w)
{
    svec sv = _mm_set_ps(w, z, y, x);
    return sv;
}

inline svec svec_new_3(float x, float y, float z)
{
    return svec_new_4(x, y, z, 0.0f);
}

inline void svec_get(svec sv, float* o_vals)
{
    _mm_store_ps(o_vals, sv);
}

#define SIMD_OP1(op)                \
    inline svec simd_##op(svec val) \
    {                               \
        return _mm_##op##_ps(val);  \
    }

SIMD_OP1(sqrt)
SIMD_OP1(rsqrt)
SIMD_OP1(rcp)

#define SIMD_OP2(op)                    \
    inline svec simd_##op(              \
        svec lhs,                       \
        svec rhs)                       \
    {                                   \
        return _mm_##op##_ps(lhs, rhs); \
    }

SIMD_OP2(add)
SIMD_OP2(sub)
SIMD_OP2(mul)
SIMD_OP2(div)

SIMD_OP2(min)
SIMD_OP2(max)

SIMD_OP2(hadd)
SIMD_OP2(hsub)
SIMD_OP2(addsub)

inline float simd_dot(svec lhs, svec rhs)
{
    __m128 mulRes, shufReg, sumsReg;
    mulRes = _mm_mul_ps(lhs, rhs);

    shufReg = _mm_movehdup_ps(mulRes);
    sumsReg = _mm_add_ps(mulRes, shufReg);
    shufReg = _mm_movehl_ps(shufReg, sumsReg);
    sumsReg = _mm_add_ss(sumsReg, shufReg);
    return _mm_cvtss_f32(sumsReg);
}

inline svec simd_cross(svec lhs, svec rhs)
{
    __m128 result = _mm_sub_ps(
        _mm_mul_ps(lhs, _mm_shuffle_ps(rhs, rhs, _MM_SHUFFLE(3, 0, 2, 1))),
        _mm_mul_ps(rhs, _mm_shuffle_ps(lhs, lhs, _MM_SHUFFLE(3, 0, 2, 1))));

    return _mm_shuffle_ps(result, result, _MM_SHUFFLE(3, 0, 2, 1));
}

void print_svec(const char *prefix, svec vec);

#endif