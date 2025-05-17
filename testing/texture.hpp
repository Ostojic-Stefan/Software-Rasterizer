#pragma once

#include "image.hpp"
#include "color.hpp"

#include <vector>

namespace rasterizer
{

	template <typename Pixel>
	struct texture
	{
		std::vector<image<Pixel>> mipmaps;

		std::uint32_t width() const
		{
			if (!mipmaps.empty())
				return mipmaps.front().width;
			return 0;
		}

		std::uint32_t height() const
		{
			if (!mipmaps.empty())
				return mipmaps.front().height;
			return 0;
		}
	};

	void generate_mipmaps(texture<color4ub> & texture);

}
