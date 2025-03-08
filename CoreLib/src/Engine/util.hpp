#pragma once

#include "pch.h"

#define BIND_EVENT_FN(x) (std::bind(&x, this, std::placeholders::_1))

namespace util
{
	template <typename T>
	static inline T max3(T x, T y, T z)
	{
		return std::max(x, std::max(y, z));
	}

	template <typename T>
	static inline T min3(T x, T y, T z)
	{
		return std::min(x, std::min(y, z));
	}
}
