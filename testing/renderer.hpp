#pragma once

#include "color.hpp"
#include "viewport.hpp"
#include "framebuffer.hpp"
#include "draw_command.hpp"

namespace rasterizer
{

	void clear(image_view<color4ub> const & color_buffer, vector4f const & color);
	void clear(image_view<std::uint32_t> const & depth_buffer, std::uint32_t value);

	void draw(framebuffer const & framebuffer, viewport const & viewport, draw_command const & command);

}
