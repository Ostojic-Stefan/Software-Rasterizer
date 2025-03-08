#pragma once

#include <format>

#define LOG(...) std::println(__VA_ARGS__)

#define ASSERT(cond, ...){ if(!(cond)) { LOG("Assertion Failed!"); LOG(__VA_ARGS__); __debugbreak(); } }

#define STATIC_ASSERT(cond, ...) (static_assert(cond, __VA_ARGS__))