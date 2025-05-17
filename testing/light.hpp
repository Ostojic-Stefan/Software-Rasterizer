#pragma once

#include "vector.hpp"

#include <span>

namespace rasterizer
{

	struct directional_light
	{
		vector3f direction;
		vector3f intensity;
	};

	struct point_light
	{
		vector3f position;
		vector3f intensity;
		vector3f attenuation;
	};

	struct light_settings
	{
		vector3f ambient_light = {0.f, 0.f, 0.f};
		std::span<directional_light const> directional_lights = {};
		std::span<point_light const> point_lights = {};
	};

}
