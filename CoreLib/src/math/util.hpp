#pragma once

#include "types.hpp"
#include <numbers>

namespace math
{
	static inline constexpr rnd::f32 pi32 = std::numbers::pi_v<rnd::f32>;
	static inline constexpr rnd::f64 pi64 = std::numbers::pi_v<rnd::f64>;

	template<typename T>
	static inline T lerp(const T& a, const T& b, rnd::f32 t)
	{
		return (1.f - t) * a + t * b;
	}

	template<typename T>
	static inline T remap(T val, T in_min, T in_max, T out_min, T out_max)
	{
		T t = (val - in_min) / (in_max - in_min);
		return lerp(out_min, out_max, t);
	}

	template <typename TVec>
	static inline rnd::f32 det_2d(const TVec& v0, const TVec& v1)
	{
		return v0.x * v1.y - v0.y * v1.x;
	}

	template <typename T>
	static inline rnd::f32 radians(const T& deg)
	{
		return deg * (180.0f / math::pi32);
	}
}