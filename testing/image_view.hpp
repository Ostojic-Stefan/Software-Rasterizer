#pragma once

#include <cstdint>

namespace rasterizer
{

	template <typename Pixel>
	struct image_view
	{
		Pixel * pixels = nullptr;
		std::uint32_t width = 0;
		std::uint32_t height = 0;

		explicit operator bool() const
		{
			return pixels != nullptr;
		}

		Pixel & at(std::uint32_t x, std::uint32_t y) const
		{
			return pixels[x + y * width];
		}
	};

}
