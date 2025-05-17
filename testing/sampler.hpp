#pragma once

namespace rasterizer
{

	enum class filtering
	{
		nearest,
		linear,
	};

	struct sampler
	{
		filtering mag_filter = filtering::nearest;
		filtering min_filter = filtering::nearest;
	};

}
