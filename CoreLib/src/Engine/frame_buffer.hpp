#pragma once

#include "pch.h"
#include "types.hpp"
#include "graphics/color.hpp"
#include "math/point.hpp"

#define MAX_DEPTH 10000

namespace rnd
{
	class framebuffer
	{
	public:
		framebuffer(u32 width, u32 height) 
			: 
			width{ width },
			height{ height },
			color_buffer{ std::make_unique<color[]>(width * height) },
			depth_buffer{ std::make_unique<u32[]>(width * height)}
		{}

		void put_pixel(u32 x, u32 y, const color& c)
		{
			ASSERT(x < width, "x < {}. x = {}", width, x);
			ASSERT(y < height, "y < {}. y = {}", height, y);
			ASSERT(x >= 0, "x >= 0. x = {}", x);
			ASSERT(y >= 0, "y >= 0. y = {}", y);

			//if (x < width && x >= 0 && y >= 0 && y < height)
			color_buffer[y * width + x] = c;	
		}

		color get_pixel(u32 x, u32 y)
		{
			ASSERT(x < width, "x < {}. x = {}", width, x);
			ASSERT(y < height, "y < {}. y = {}", height, y);
			ASSERT(x >= 0, "x >= 0. x = {}", x);
			ASSERT(y >= 0, "y >= 0. y = {}", y);

			//if (x < width && x >= 0 && y >= 0 && y < height)
			return color_buffer[y * width + x];
		}

		void clear_color(const color& c)
		{
			std::fill(color_buffer.get(), color_buffer.get() + (width * height), c);
		}

		inline void reset(u32 width, u32 height)
		{
			this->width = width;
			this->height = height;

			color_buffer = std::make_unique<color[]>(width * height);
			depth_buffer = std::make_unique<u32[]>(width * height);
		}

		inline u32 get_width()	const { return width; }
		inline u32 get_height() const { return height; }

		inline math::pt2i get_dimensions() const { return { (rnd::i32) width, (rnd::i32) height }; }

		inline const color* get_color_buffer()	const { return color_buffer.get(); }

		inline const rnd::u32 get_depth(rnd::u32 x, rnd::u32 y) const
		{
			ASSERT(x < width, "x < {}. x = {}", width, x);
			ASSERT(y < height, "y < {}. y = {}", height, y);
			ASSERT(x >= 0, "x >= 0. x = {}", x);
			ASSERT(y >= 0, "y >= 0. y = {}", y);

			return depth_buffer[y * width + x];
		}

		inline void set_depth(rnd::u32 x, rnd::u32 y, rnd::u32 depth)
		{
			ASSERT(x < width, "x < {}. x = {}", width, x);
			ASSERT(y < height, "y < {}. y = {}", height, y);
			ASSERT(x >= 0, "x >= 0. x = {}", x);
			ASSERT(y >= 0, "y >= 0. y = {}", y);

			depth_buffer[y * width + x] = depth;
		}

		void clear_depth()
		{
			std::fill(depth_buffer.get(), depth_buffer.get() + (width * height), MAX_DEPTH);
		}

		inline const u32* get_depth_buffer() const { return depth_buffer.get(); }

	private:
		u32 width, height;
		std::unique_ptr<color[]> color_buffer;
		std::unique_ptr<u32[]> depth_buffer;
	};

}
