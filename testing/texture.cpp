#include "texture.hpp"

namespace rasterizer
{

	void generate_mipmaps(texture<color4ub> & texture)
	{
		if (texture.mipmaps.empty())
			return;

		texture.mipmaps.resize(1);

		for (int i = 1;; ++i)
		{
			auto & prev_level = texture.mipmaps[i - 1];

			if (prev_level.width == 1 && prev_level.height == 1)
				break;

			std::uint32_t new_width = prev_level.width / 2 + (prev_level.width & 1);
			std::uint32_t new_height = prev_level.height / 2 + (prev_level.height & 1);

			image<color4ub> next_level = image<color4ub>::allocate(new_width, new_height);

			auto get_pixel = [&](std::uint32_t x, std::uint32_t y)
			{
				return to_vector4f(prev_level.at(std::min(x, prev_level.width - 1), std::min(y, prev_level.height - 1)));
			};

			for (std::uint32_t y = 0; y < new_height; ++y)
			{
				for (std::uint32_t x = 0; x < new_width; ++x)
				{
					vector4f result{0.f, 0.f, 0.f, 0.f};

					result = result + get_pixel(2 * x + 0, 2 * y + 0);
					result = result + get_pixel(2 * x + 1, 2 * y + 0);
					result = result + get_pixel(2 * x + 0, 2 * y + 1);
					result = result + get_pixel(2 * x + 1, 2 * y + 1);

					result = result / 4.f;

					next_level.at(x, y) = to_color4ub(result);
				}
			}

			texture.mipmaps.push_back(std::move(next_level));
		}
	}

}
