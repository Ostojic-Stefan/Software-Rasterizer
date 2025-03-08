#include "pch.h"
#include "timer.hpp"

namespace rnd
{
	f32 timer::get_elapsed_s() const
	{
		return get_elapsed_ms() / 1000.f;
	}

	f32 timer::get_elapsed_ms() const
	{
		auto now = std::chrono::high_resolution_clock::now();
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - prev_time);
		prev_time = now;
		return std::chrono::duration<f32, std::milli>(ms).count();
	}
}