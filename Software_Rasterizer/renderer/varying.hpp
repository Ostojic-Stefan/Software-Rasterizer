#pragma once

#include "math/vector.hpp"
#include "generic_value.hpp"

#include <array>

struct VSInput
{
	void Set(size_t slot, GenericValue value)
	{
		assert(slot < MAX_ATTRIBS);
		attribs[slot] = value;
	}

	template <typename T>
	T Get(size_t slot) const;

private:
	static constexpr size_t MAX_ATTRIBS = 16;
	std::array<GenericValue, MAX_ATTRIBS> attribs;
};

template <>
inline math::vec2 VSInput::Get<math::vec2>(size_t slot) const
{
	assert(slot < MAX_ATTRIBS);
	GenericValue attrib = attribs[slot];
	assert(attrib.count == 2);

	return { attrib.vals[0], attrib.vals[1] };
}

template <>
inline math::vec3 VSInput::Get<math::vec3>(size_t slot) const
{
	assert(slot < MAX_ATTRIBS);
	GenericValue attrib = attribs[slot];
	assert(attrib.count == 3);

	return { attrib.vals[0], attrib.vals[1], attrib.vals[2] };
}

template <>
inline math::vec4 VSInput::Get<math::vec4>(size_t slot) const
{
	assert(slot < MAX_ATTRIBS);
	GenericValue attrib = attribs[slot];
	assert(attrib.count == 4);

	return { attrib.vals[0], attrib.vals[1], attrib.vals[2], attrib.vals[3] };
}

struct VSOutput
{
	template <typename V>
	void setVarying(uint32_t location, V var)
	{
		assert(location < MAX_VARYINGS);
		assert(location == used && "location must be the next unused slot");

		if constexpr (std::is_same_v<V, math::vec2>)
		{
			GenericValue val = {};
			val.count = 2;
			memcpy(val.vals, &var, sizeof(V));
			varyings[location] = val;
		}
		else if constexpr (std::is_same_v<V, math::vec3>)
		{
			GenericValue val = {};
			val.count = 3;
			memcpy(val.vals, &var, sizeof(V));
			varyings[location] = val;
		}
		else
		{
			assert(false && "Not implemented yet");
		}

		used += 1;
	}

	const size_t Size() const
	{
		return used;
	}

	template <typename T>
	T getVarying(uint32_t loc) const;

	math::vec4 Position = {};

	size_t used = 0;
	static constexpr size_t MAX_VARYINGS = 8;
	std::array<GenericValue, MAX_VARYINGS> varyings = { 0 };
};

template <>
inline math::vec2 VSOutput::getVarying(uint32_t loc) const
{
	assert(loc < MAX_VARYINGS);
	const GenericValue& val = varyings[loc];
	assert(val.count == 2);
	math::vec2 result;
	memcpy(&result, val.vals, sizeof(float) * val.count);
	return result;
}

template <>
inline math::vec3 VSOutput::getVarying(uint32_t loc) const
{
	assert(loc < MAX_VARYINGS);
	const GenericValue& val = varyings[loc];
	assert(val.count == 3);
	math::vec3 result;
	memcpy(&result, val.vals, sizeof(float) * val.count);
	return result;
}

template <>
inline math::vec4 VSOutput::getVarying(uint32_t loc) const
{
	assert(loc < MAX_VARYINGS);
	const GenericValue& val = varyings[loc];
	assert(val.count == 4);
	math::vec4 result;
	memcpy(&result, val.vals, sizeof(float) * val.count);
	return result;
}