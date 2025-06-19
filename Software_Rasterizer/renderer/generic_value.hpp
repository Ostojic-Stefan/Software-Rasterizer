#pragma once
#include <immintrin.h>

struct GenericValue
{
	union {
		float vals[4];
		__m128 _v;
	};
	size_t count;
};

static inline GenericValue Interpolate(const GenericValue& a0, const GenericValue& a1, const GenericValue& a2, float alpha, float beta, float gamma, float z)
{
	size_t cnt = a0.count;

	GenericValue result = { 0 };
	result.count = cnt;

	for (size_t i = 0; i < cnt; ++i)
	{
		result.vals[i] += (a0.vals[i] * alpha + a1.vals[i] * beta + a2.vals[i] * gamma) * z;
	}
	return result;
}

//static inline GenericValue Interpolate_SIMD(
//    const GenericValue& a0,
//    const GenericValue& a1,
//    const GenericValue& a2,
//    float               alpha,
//    float               beta,
//    float               gamma)
//{
//    // load the three input vectors
//    __m128 v0 = a0._v;
//    __m128 v1 = a1._v;
//    __m128 v2 = a2._v;
//
//    // broadcast the scalars into all 4 lanes
//    __m128 A = _mm_set1_ps(alpha);
//    __m128 B = _mm_set1_ps(beta);
//    __m128 C = _mm_set1_ps(gamma);
//
//    // compute (v0*A + v1*B + v2*C) in parallel
//    __m128 sum = _mm_add_ps(
//        _mm_add_ps(_mm_mul_ps(v0, A),
//            _mm_mul_ps(v1, B)),
//        _mm_mul_ps(v2, C));
//
//    GenericValue result;
//    result._v = sum;      // writes all 4 floats
//    result.count = a0.count;        // we know there are exactly 4 components
//    return result;
//}