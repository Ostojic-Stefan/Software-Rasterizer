#pragma once

namespace rasterizer
{

	enum class cull_mode
	{
		none,
		cw,
		ccw,
	};

	enum class depth_test_mode
	{
		never,
		always,
		less,
		less_equal,
		greater,
		greater_equal,
		equal,
		not_equal,
	};

	struct depth_settings
	{
		bool write = true;
		depth_test_mode mode = depth_test_mode::always;
	};

}
