#pragma once

#include "vector.hpp"

#include <cstdint>

namespace rasterizer
{

	struct viewport
	{
		std::int32_t xmin, ymin, xmax, ymax;
	};

	inline vector4f apply(viewport const & viewport, vector4f v)
	{
		v.x = viewport.xmin + (viewport.xmax - viewport.xmin) * (0.5f + 0.5f * v.x);
		v.y = viewport.ymin + (viewport.ymax - viewport.ymin) * (0.5f - 0.5f * v.y);
		return v;
	}

}
