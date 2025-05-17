#pragma once

#include "image_view.hpp"
#include "color.hpp"

#include <cstdint>
#include <memory>
#include <filesystem>

namespace rasterizer
{

	template <typename Pixel>
	struct image
	{
		std::unique_ptr<Pixel[]> pixels;
		std::uint32_t width = 0;
		std::uint32_t height = 0;

		explicit operator bool() const
		{
			return pixels != nullptr;
		}

		image_view<Pixel> view()
		{
			return image_view<Pixel>
			{
				.pixels = pixels.get(),
				.width = width,
				.height = height,
			};
		}

		Pixel & at(std::uint32_t x, std::uint32_t y) const
		{
			return pixels[x + y * width];
		}

		static image allocate(std::uint32_t width, std::uint32_t height)
		{
			return image
			{
				.pixels = std::unique_ptr<Pixel[]>(new Pixel[width * height]),
				.width = width,
				.height = height,
			};
		}
	};

	image<color4ub> load_image(std::string const & path);

}
