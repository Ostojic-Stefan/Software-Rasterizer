#pragma once

#include <Engine/engine.hpp>

struct viewport
{
	std::int32_t xmin, ymin, xmax, ymax;

	/// <summary>
	/// Transforms the point specified in NDC Space (0,0) is middle,
	/// to the range specified by the bounds (xmin, ymin, xmax, ymax)
	/// </summary>
	/// <param name="pt">The point that will be transformed</param>
	/// <returns>The Transformed point</returns>
	math::vec4 transform(math::vec4 pt) const
	{
		pt.x = xmin + (xmax - xmin) * (0.5f + 0.5f * pt.x);
		pt.y = ymin + (ymax - ymin) * (0.5f - 0.5f * pt.y);
		return pt;
	}
};