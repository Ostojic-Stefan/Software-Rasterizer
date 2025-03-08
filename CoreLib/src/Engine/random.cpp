#pragma once
#include "pch.h"
#include "random.hpp"

namespace rnd
{
	std::random_device random::dev;
	std::mt19937 random::engine(random::dev());
}
