#pragma once

#include "vector.hpp"

#include <cstdint>
#include <cmath>

namespace rasterizer
{

	struct color4ub
	{
		std::uint8_t r, g, b, a;
	};

	// Define custom min/max so that we don't have to include the full <algorithm> header

	template<typename T>
	T max(T a, T b)
	{
		return (a > b) ? a : b;
	}

	template<typename T>
	T min(T a, T b)
	{
		return (a > b) ? b : a;
	}

	inline color4ub to_color4ub(vector4f const & c)
	{
		color4ub result;

		result.r = max(0.f, min(255.f, c.x * 255.f));
		result.g = max(0.f, min(255.f, c.y * 255.f));
		result.b = max(0.f, min(255.f, c.z * 255.f));
		result.a = max(0.f, min(255.f, c.w * 255.f));

		return result;
	}

	inline vector4f to_vector4f(color4ub const & c)
	{
		return vector4f{(float)c.r, (float)c.g, (float)c.b, (float)c.a} / 255.f;
	}

}
