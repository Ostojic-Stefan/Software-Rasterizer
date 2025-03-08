#pragma once

#include "types.hpp"
#include "graphics.hpp"

#include <memory>
#include <string_view>

namespace gfx
{
	class texture
	{
	public:
		static texture from_file(std::string_view file_path);

		rnd::color at(rnd::u32 x, rnd::u32 y) const;
		math::vec4 sample(rnd::f32 x, rnd::f32 y) const;


		rnd::i32 get_width() const;
		rnd::i32 get_height() const;

	private:
		std::unique_ptr<rnd::color[]> data;
		rnd::u32 width;
		rnd::u32 height;
	};
}