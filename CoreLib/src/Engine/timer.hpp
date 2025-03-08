#pragma once

#include "pch.h"
#include "types.hpp"

namespace rnd
{
	class timer
	{
	public:
		timer() { prev_time = std::chrono::high_resolution_clock::now(); }
		f32 get_elapsed_s() const;
		f32 get_elapsed_ms() const;
	private:
		mutable std::chrono::steady_clock::time_point prev_time;
	};
}
