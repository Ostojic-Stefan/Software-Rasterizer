#pragma once

#include <cstdint>

namespace rasterizer
{

	template <typename T>
	struct attribute
	{
		void const * pointer = nullptr;
		std::uint32_t stride = sizeof(T);

		T const & operator[] (std::uint32_t i) const
		{
			return *(T const *)((char const *)(pointer) + stride * i);
		}
	};

}
