#include "pch.h"
#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

namespace gfx
{
	texture texture::from_file(std::string_view file_path)
	{
		texture res;
		rnd::i32 width, height, channels;
		rnd::color* result = reinterpret_cast<rnd::color*>(stbi_load(file_path.data(), &width, &height, &channels, 4));

		if (!result) {
			throw new std::runtime_error("Failed to load image: " + std::string(file_path));
		}

		res.width = width;
		res.height = height;
		res.data = std::make_unique<rnd::color[]>(width * height);

		std::copy(result, result + (width * height), res.data.get());

		stbi_image_free(result);

		return res;
	}

	math::vec4 texture::sample(rnd::f32 x, rnd::f32 y) const
	{
		rnd::u32 x_coord = (rnd::u32)std::max(0.f, std::min(width - 1.f, x * width));
		rnd::u32 y_coord = (rnd::u32)std::max(0.f, std::min(height - 1.f, y * height));

		rnd::color t = at(x_coord, y_coord);
		return rnd::to_vec4(t);
	}

	rnd::color texture::at(rnd::u32 x, rnd::u32 y) const
	{
		return data[y * width + x];
	}

	rnd::i32 texture::get_width() const { return width; }

	rnd::i32 texture::get_height() const { return height; }
}

