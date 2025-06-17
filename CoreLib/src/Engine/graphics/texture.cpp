#include "pch.h"
#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

namespace gfx
{
	surface::surface(rnd::u32 width, rnd::u32 height)
		:
		data(std::make_unique<rnd::color[]>(width * height)),
		width(width),
		height(height)
	{}

	surface surface::from_file(std::string_view file_path)
	{
		rnd::i32 width, height, channels;
		rnd::color* result = reinterpret_cast<rnd::color*>(stbi_load(file_path.data(), &width, &height, &channels, 4));

		if (!result)
		{
			const char* reason = stbi_failure_reason();
			LOG("{}", reason);
			throw new std::runtime_error("Failed to load image: " + std::string(file_path));
		}
	
		surface res(width, height);

		std::copy(result, result + (width * height), res.data.get());

		stbi_image_free(result);

		return res;
	}

	math::vec4 surface::sample(rnd::f32 x, rnd::f32 y) const
	{
		rnd::u32 x_coord = (rnd::u32)std::max(0.f, std::min(width - 1.f, x * width));
		rnd::u32 y_coord = (rnd::u32)std::max(0.f, std::min(height - 1.f, y * height));

		rnd::color t = at(x_coord, y_coord);
		return rnd::to_vec4(t);
	}

	rnd::color& surface::at(rnd::u32 x, rnd::u32 y) const
	{
		ASSERT(x < width, "x < {}. x = {}", width, x);
		ASSERT(y < height, "y < {}. y = {}", height, y);
		ASSERT(x >= 0, "x >= 0. x = {}", x);
		ASSERT(y >= 0, "y >= 0. y = {}", y);

		return data[y * width + x];
	}

	rnd::i32 surface::get_width() const { return width; }

	rnd::i32 surface::get_height() const { return height; }

	texture texture::from_file(std::string_view file_path)
	{
		surface surf = surface::from_file(file_path);

		texture res;
		res.mips.push_back(std::move(surf));

		return res;
	}

	void texture::generate_mipmaps()
	{
		rnd::sz prev_mip = 0;
		math::vec2i dims{ mips[prev_mip].get_width(), mips[prev_mip].get_height() };

		while (dims.x != 1 && dims.y != 1)
		{
			rnd::i32 width = dims.x / 2 + (dims.x & 1);
			rnd::i32 height = dims.y / 2 + (dims.y & 1);

			surface surf(width, height);

			for (rnd::i32 y = 0; y < height; ++y)
			{
				for (rnd::i32 x = 0; x < width; ++x)
				{
					const surface& prev = mips[prev_mip];
					math::vec4 accum(0.f);

					accum += rnd::to_vec4(prev.at(x * 2 + 0, y * 2 + 0));
					accum += rnd::to_vec4(prev.at(x * 2 + 1, y * 2 + 0));
					accum += rnd::to_vec4(prev.at(x * 2 + 0, y * 2 + 1));
					accum += rnd::to_vec4(prev.at(x * 2 + 1, y * 2 + 1));

					accum = accum / 4.f;

					surf.at(x, y) = rnd::to_color(accum);
				}
			}
			mips.push_back(std::move(surf));
			prev_mip += 1;
			dims = {width, height};
		}
	}

	void texture::set_mip_level(rnd::sz mip_level)
	{
		curr_mip_level = mip_level;
	}

	rnd::color& texture::at(rnd::u32 x, rnd::u32 y) const
	{
		ASSERT(curr_mip_level < mips.size(), "invalid mip level");
		return mips[curr_mip_level].at(x, y);
	}

	math::vec4 texture::sample(rnd::f32 x, rnd::f32 y) const
	{
		ASSERT(curr_mip_level < mips.size(), "invalid mip level");
		return mips[curr_mip_level].sample(x, y);
	}

	rnd::i32 texture::get_width() const
	{
		ASSERT(curr_mip_level < mips.size(), "invalid mip level");
		return mips[curr_mip_level].get_width();
	}

	rnd::i32 texture::get_height() const
	{
		ASSERT(curr_mip_level < mips.size(), "invalid mip level");
		return mips[curr_mip_level].get_height();
	}
}

