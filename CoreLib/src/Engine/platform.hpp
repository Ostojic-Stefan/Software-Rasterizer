#pragma once

#include <string_view>
#include "types.hpp"
#include "frame_buffer.hpp"

namespace platform
{
	struct config {
		std::string_view	title;
		rnd::i32			width;
		rnd::i32			height;
	};

	rnd::b8 initialize(const config& window_config);
	void process_events();
	void shutdown();
	void display_framebuffer(const rnd::framebuffer& fb);
	//void swap_buffers();
	//rnd::b8 set_vsync(rnd::b8 flag);
}
