#pragma once

#include "types.hpp"
#include "generic_value.hpp"

#include <vector>

enum class AttribType { Float };
struct VertexAttrib
{
	AttribType type;
	size_t elementCount;
	rnd::u32 offset;
	size_t slot;
};

struct VertexBuffer
{
	VertexBuffer(const rnd::u8* data, rnd::sz stride)
		: data(data), stride(stride) {}

	void add_attrib(VertexAttrib attrib)
	{
		attribs.push_back(std::move(attrib));
	}

	const std::vector<VertexAttrib>& get_attribs() const
	{
		return attribs;
	}

	const uint8_t* get_data() const { return data; }

	const rnd::sz get_stride() const { return stride; }

private:
	const uint8_t* data;
	size_t stride;
	std::vector<VertexAttrib> attribs;
};

static inline GenericValue extract_vertex_attribute(const uint8_t* ptr, const VertexAttrib& attrib)
{
	assert(attrib.type == AttribType::Float);

	GenericValue result = {};
	result.count = attrib.elementCount;

	memcpy(result.vals, ptr, attrib.elementCount * sizeof(float));
	return result;
}

struct IndexBuffer
{
	const rnd::u16* data = nullptr;
	rnd::sz count = 0;
};
