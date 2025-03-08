#pragma once

#include "pch.h"
#include "types.hpp"
#include "math/vector.hpp"
#include "util.hpp"
#include "random.hpp"

#include "math/vector.hpp"

namespace rnd
{
	struct color
	{
		u8 r, g, b, a;
	};

	static constexpr inline color to_color(const math::vec4& c)
	{
		return color {
			.r = (u8) std::clamp(c.x * 255.f, 0.f, 255.f),
			.g = (u8) std::clamp(c.y * 255.f, 0.f, 255.f),
			.b = (u8) std::clamp(c.z * 255.f, 0.f, 255.f),
			.a = (u8) std::clamp(c.w * 255.f, 0.f, 255.f)
		};
	}

	static constexpr inline color to_color(const math::vec3& c)
	{
		return color{
			.r = (u8)std::clamp(c.x * 255.f, 0.f, 255.f),
			.g = (u8)std::clamp(c.y * 255.f, 0.f, 255.f),
			.b = (u8)std::clamp(c.z * 255.f, 0.f, 255.f),
			.a = 255
		};
	}

	static inline math::vec3 to_vec3(const color& c)
	{
		return math::vec3{ (f32)c.r, (f32)c.g, (f32)c.b } / 255.f;
	}

	static inline math::vec4 to_vec4(const color& c)
	{
		return math::vec4{ (f32)c.r, (f32)c.g, (f32)c.b, (f32)c.a } / 255.f;
	}

	static inline color get_random_color()
	{
		rnd::random rand;
		rnd::u32 r = rand.get<rnd::u32>(0, 255);
		rnd::u32 g = rand.get<rnd::u32>(0, 255);
		rnd::u32 b = rand.get<rnd::u32>(0, 255);

		return { (rnd::u8)r, (rnd::u8)g, (rnd::u8)b, 255 };
	}

	static constexpr color black	=	color{ .r = 0,		.g = 0,		.b = 0,		.a = 255 };
	static constexpr color white	=	color{ .r = 255,	.g = 255,	.b = 255,	.a = 255 };
	static constexpr color red		=	color{ .r = 255,	.g = 0,		.b = 0,		.a = 255 };
	static constexpr color green	=	color{ .r = 0,		.g = 255,	.b = 0,		.a = 255 };
	static constexpr color blue		=	color{ .r = 0,		.g = 0,		.b = 255,	.a = 255 };
	static constexpr color magenta	=	color{ .r = 255,	.g = 0,		.b = 255,	.a = 255 };
	static constexpr color yellow	=	color{ .r = 255,	.g = 255,	.b = 0,		.a = 255 };
	static constexpr color cyan		=	color{ .r = 0,		.g = 255,	.b = 255,	.a = 255 };

	static constexpr color dark_gray = to_color(math::vec3({ 0.1f, 0.1f, 0.1f }));

}