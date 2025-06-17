#pragma once

#include <immintrin.h>
#include <cassert>
#include <cstdint>

namespace simd
{

	struct vInt
	{
	public:
		static constexpr uint32_t Length = sizeof(__m256i) / sizeof(int32_t);

		vInt() : vec{ _mm256_set1_epi32(0) } {}
		vInt(__m256i x) : vec{ x } {}
		vInt(int32_t x) : vec{ _mm256_set1_epi32(x) } {}

		inline operator __m256i() const { return vec; }

		inline int32_t& operator[](size_t idx) const {
			assert(idx >= 0 && idx < Length);
			return ((int32_t*)&vec)[idx];
		}
		inline void store(void* ptr) const { _mm256_storeu_si256(static_cast<__m256i*>(ptr), vec); }
		static inline vInt load(const void* ptr) { return _mm256_loadu_si256(static_cast<const __m256i*>(ptr)); }
		static inline vInt ramp() { return _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7); }

	public:
		__m256i vec;
	};

	struct vFloat
	{
	public:
		static constexpr uint32_t Length = sizeof(__m256) / sizeof(float);

		vFloat() : vec{ _mm256_setzero_ps() } {}
		vFloat(__m256 x) : vec{ x } {}
		vFloat(float x) : vec{ _mm256_set1_ps(x) } {}
		inline operator __m256() const { return vec; }
		inline operator vInt() { return vInt(_mm256_cvtps_epi32(vec)); }
		inline float& operator[](size_t idx) const
		{
			assert(idx >= 0 && idx < Length);
			return ((float*)&vec)[idx];
		}
		inline void store(void* ptr) const { _mm256_storeu_ps(static_cast<float*>(ptr), vec); }
		static inline vFloat load(const void* ptr) { return _mm256_loadu_ps(static_cast<const float*>(ptr)); }
	public:
		__m256 vec;
	};

	struct vFloat2 {
		vFloat x, y;

		vFloat2() = default;
		vFloat2(float v) { x = y = v; }
		vFloat2(vFloat v) { x = y = v; }
		vFloat2(vFloat x_, vFloat y_) { x = x_, y = y_; }
		//vFloat2(const glm::vec2& v) { x = v.x, y = v.y; }
	};

	struct vFloat3 {
		vFloat x, y, z;

		vFloat3() = default;
		vFloat3(float v) { x = y = z = v; }
		vFloat3(vFloat v) { x = y = z = v; }
		vFloat3(vFloat x_, vFloat y_, vFloat z_) { x = x_, y = y_, z = z_; }
		//VFloat3(const glm::vec3& v) { x = v.x, y = v.y, z = v.z; }
		//explicit VFloat3(const VFloat4& v);
	};

	struct vFloat4 {
		vFloat x, y, z, w;

		vFloat4() = default;
		vFloat4(float v) { x = y = z = w = v; }
		vFloat4(vFloat v) { x = y = z = w = v; }
		vFloat4(vFloat x_, vFloat y_, vFloat z_, vFloat w_) { x = x_, y = y_, z = z_, w = w_; }
		vFloat4(vFloat3 a, vFloat w_) { x = a.x, y = a.y, z = a.z, w = w_; }
		//vFloat4(const glm::vec4& v) { x = v.x, y = v.y, z = v.z, w = v.w; }
	};



	// float operators
	inline vFloat operator+(vFloat a, vFloat b) { return _mm256_add_ps(a, b); }
	inline vFloat operator-(vFloat a, vFloat b) { return _mm256_sub_ps(a, b); }
	inline vFloat operator*(vFloat a, vFloat b) { return _mm256_mul_ps(a, b); }
	inline vFloat operator&(vFloat a, vFloat b) { return _mm256_and_ps(a, b); }
	inline vFloat operator|(vFloat a, vFloat b) { return _mm256_or_ps(a, b); }
	inline vFloat operator^(vFloat a, vFloat b) { return _mm256_xor_ps(a, b); }
	inline vFloat operator+=(vFloat& a, vFloat b) { return a = (a + b); }
	inline vFloat operator-=(vFloat& a, vFloat b) { return a = (a - b); }
	inline vFloat operator*=(vFloat& a, vFloat b) { return a = (a * b); }
	inline vFloat operator < (vFloat a, vFloat b) { return	_mm256_cmp_ps(a, b, _CMP_LT_OQ); }
	inline vFloat operator > (vFloat a, vFloat b) { return	_mm256_cmp_ps(a, b, _CMP_GT_OQ); }
	inline vFloat operator <= (vFloat a, vFloat b) { return _mm256_cmp_ps(a, b, _CMP_LE_OQ); }
	inline vFloat operator >= (vFloat a, vFloat b) { return _mm256_cmp_ps(a, b, _CMP_GE_OQ); }
	inline vFloat operator == (vFloat a, vFloat b) { return _mm256_cmp_ps(a, b, _CMP_EQ_OQ); }
	inline vFloat operator!=(vFloat a, vFloat b) { return	_mm256_cmp_ps(a, b, _CMP_NEQ_UQ); }

	inline vFloat operator/(vFloat a, vFloat b) { return _mm256_div_ps(a, b); }
	inline vFloat operator-(vFloat a) { return a ^ -0.0f; }


	inline vFloat4 operator+(vFloat4 a, vFloat4 b) { return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w }; }
	inline vFloat4 operator-(vFloat4 a, vFloat4 b) { return { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w }; }
	inline vFloat4 operator*(vFloat4 a, vFloat4 b) { return { a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w }; }
	inline vFloat4 operator/(vFloat4 a, vFloat4 b) { return { a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w }; }


	struct vInt2
	{
		vInt x, y;

		vInt2() = default;
		vInt2(int32_t v) { x = y = v; }
		vInt2(vInt v) { x = y = v; }
		vInt2(vInt x_, vInt y_) { x = x_, y = y_; }
	};


	struct vInt3
	{
		vInt x, y, z;

		vInt3() = default;
		vInt3(int32_t v) { x = y = z = v; }
		vInt3(vInt v) { x = y = z = v; }
		vInt3(vInt x_, vInt y_, vInt z_) { x = x_, y = y_, z = z_; }
	};

	// int operators
	inline vInt operator+(vInt a, vInt b) { return _mm256_add_epi32(a, b); }
	inline vInt operator-(vInt a, vInt b) { return _mm256_sub_epi32(a, b); }
	inline vInt operator*(vInt a, vInt b) { return _mm256_mullo_epi32(a, b); }
	inline vInt operator&(vInt a, vInt b) { return _mm256_and_si256(a, b); }
	inline vInt operator|(vInt a, vInt b) { return _mm256_or_si256(a, b); }
	inline vInt operator^(vInt a, vInt b) { return _mm256_xor_si256(a, b); }
	inline vInt operator+=(vInt& a, vInt b) { return a = (a + b); }
	inline vInt operator-=(vInt& a, vInt b) { return a = (a - b); }
	inline vInt operator*=(vInt& a, vInt b) { return a = (a * b); }
	inline uint8_t operator<(vInt a, vInt b) { return  _mm256_movemask_epi8(_mm256_cmpgt_epi32(b, a)); }
	inline uint8_t operator>(vInt a, vInt b) { return  _mm256_movemask_epi8(_mm256_cmpgt_epi32(a, b)); }
	inline uint8_t operator<=(vInt a, vInt b)
	{
		uint8_t gt = operator>(a, b);
		return (uint8_t)((~gt) & 0xFF);
	}
	inline uint8_t operator>=(vInt a, vInt b)
	{
		uint8_t lt = operator<(a, b);
		return (uint8_t)((~lt) & 0xFF);
	}
	inline uint8_t operator==(vInt a, vInt b) { return _mm256_movemask_epi8(_mm256_cmpeq_epi32(a, b)); }
	inline uint8_t operator!=(vInt a, vInt b)
	{
		uint8_t eq = operator==(a, b);
		return (uint8_t)((~eq) & 0xFF);
	}

	inline vInt operator>>(vInt a, uint32_t b) { return _mm256_srai_epi32(a, b); }
	inline vInt operator<<(vInt a, uint32_t b) { return _mm256_slli_epi32(a, b); }
	inline vInt operator>>(vInt a, vInt b) { return _mm256_srav_epi32(a, b); }
	inline vInt operator<<(vInt a, vInt b) { return _mm256_sllv_epi32(a, b); }

	// helpers
	inline vInt min(vInt x, vInt y) { return _mm256_min_epi32(x, y); }
	inline vInt max(vInt x, vInt y) { return _mm256_max_epi32(x, y); }
	inline vFloat min(vFloat x, vFloat y) { return _mm256_min_ps(x, y); }
	inline vFloat max(vFloat x, vFloat y) { return _mm256_max_ps(x, y); }

	inline vFloat4 perspective_divide(const vFloat4& v)
	{
		vFloat rw = 1.f / v.w;
		return { v.x * rw, v.y * rw, v.z * rw, rw };
	}

	inline vFloat floor(const vFloat& v)
	{
		return _mm256_floor_ps(v);
	}

	inline vFloat dot(const vFloat3& a, const vFloat3& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	inline vFloat dot(const vFloat4& a, const vFloat4& b) {
		return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	}

	// Normalize a vFloat3 vector (using AVX sqrt)
	inline vFloat3 normalize(const vFloat3& v) {
		vFloat d = dot(v, v);
		d = vFloat(_mm256_sqrt_ps(d.vec));
		return vFloat3(v.x / d, v.y / d, v.z / d);
	}


	//inline float det2DGLM(glm::vec2 const& v0, glm::vec2 const& v1)
	//{
	//	return v0.x * v1.y - v0.y * v1.x;
	//}
	inline vFloat det2(const vFloat2& v0, const vFloat2& v1)
	{
		return v0.x * v1.y - v0.y * v1.x;
	}

	inline vInt det2(const vInt2& v0, const vInt2& v1)
	{
		return v0.x * v1.y - v0.y * v1.x;
	}

	// for convenience
	inline vFloat det2(const vFloat4& v0, const vFloat4& v1)
	{
		return v0.x * v1.y - v0.y * v1.x;
	}

	inline vFloat blend(const vFloat& a, const vFloat& b, const vFloat& mask)
	{
		return vFloat(_mm256_blendv_ps(a.vec, b.vec, mask.vec));
	}

	inline vFloat2 blend(const vFloat2& a, const vFloat2& b, const vFloat& mask)
	{
		return vFloat2(blend(a.x, b.x, mask), blend(a.y, b.y, mask));
	}

	inline vFloat3 blend(const vFloat3& a, const vFloat3& b, const vFloat& mask)
	{
		return vFloat3(blend(a.x, b.x, mask), blend(a.y, b.y, mask), blend(a.z, b.z, mask));
	}

	inline vFloat4 blend(const vFloat4& a, const vFloat4& b, const vFloat& mask)
	{
		return vFloat4(blend(a.x, b.x, mask), blend(a.y, b.y, mask), blend(a.z, b.z, mask), blend(a.w, b.w, mask));
	}

	inline static void transpose8x3(__m256i v[3])
	{
		auto a = _mm256_blend_epi32(_mm256_blend_epi32(v[1], v[0], 0b01'001'001), v[2], 0b00'100'100);
		auto b = _mm256_blend_epi32(_mm256_blend_epi32(v[0], v[2], 0b01'001'001), v[1], 0b00'100'100);
		auto c = _mm256_blend_epi32(_mm256_blend_epi32(v[2], v[1], 0b01'001'001), v[0], 0b00'100'100);

		v[0] = _mm256_permutevar8x32_epi32(a, _mm256_setr_epi32(0, 3, 6, 1, 4, 7, 2, 5));
		v[1] = _mm256_permutevar8x32_epi32(b, _mm256_setr_epi32(1, 4, 7, 2, 5, 0, 3, 6));
		v[2] = _mm256_permutevar8x32_epi32(c, _mm256_setr_epi32(2, 5, 0, 3, 6, 1, 4, 7));
	}

	template <typename TVec>
	inline static TVec clamp(TVec val, TVec lo, TVec hi)
	{
		return simd::max(lo, simd::min(val, hi));
	}

	inline static void swap_cond(vInt& v1, vInt& v2, vInt mask)
	{
		vInt temp = v1;
		v1 = _mm256_blendv_epi8(v1, v2, mask);
		v2 = _mm256_blendv_epi8(v2, temp, mask);
	}

	inline vInt round2i(vFloat x) { return _mm256_cvtps_epi32(x.vec); }
	inline vInt trunc2i(vFloat x) { return _mm256_cvttps_epi32(x.vec); }
	inline vFloat conv2f(vInt x) { return _mm256_cvtepi32_ps(x.vec); }
}