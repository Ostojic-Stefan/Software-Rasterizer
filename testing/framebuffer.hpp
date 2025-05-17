#pragma once

#include "image_view.hpp"
#include "color.hpp"

namespace rasterizer
{

	struct framebuffer
	{
		image_view<color4ub> color;
		image_view<std::uint32_t> depth;

		std::uint32_t width() const
		{
			if (color)
				return color.width;
			return depth.width;
		}

		std::uint32_t height() const
		{
			if (color)
				return color.height;
			return depth.height;
		}
	};

}
