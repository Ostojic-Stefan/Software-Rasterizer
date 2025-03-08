#pragma once

#include "pch.h"
#include "types.hpp"

namespace rnd
{
	struct random
	{
	public:
		template <typename T>
		T get(T min, T max)
		{
			if constexpr (std::is_integral_v<T>)
			{
				std::uniform_int_distribution<T> dist(min, max);
				return dist(engine);
			}
			else if constexpr (std::is_floating_point_v<T>)
			{
				std::uniform_real_distribution<T> dist(min, max);
				return dist(engine);
			}
			ASSERT(false, "random. unsuppored type provided");
		}
	private:
		static std::mt19937 engine;
		static std::random_device dev;
	};
}
