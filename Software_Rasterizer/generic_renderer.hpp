#pragma once

#include <vector>
#include <cassert>
#include <cstring>
#include <array>
#include <type_traits>
#include <functional>

#include "types.hpp"
#include "math/vector.hpp"

#include "viewport.hpp"

struct GenericValue
{
	float vals[4];
	int count;
};

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
	uint8_t* data;
	size_t stride;

	std::vector<VertexAttrib> attribs;
};

static inline GenericValue ExtractValue(const uint8_t* ptr, const VertexAttrib& attribute)
{
	GenericValue result = {};
	result.count = attribute.elementCount;
	if (attribute.type == AttribType::Float)
	{
		memcpy(result.vals, ptr, attribute.elementCount * sizeof(float));
	}
	else
	{
		assert("attrib type not yet implemented");
	}
	return result;
}

struct VsInput
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
inline math::vec2 VsInput::Get<math::vec2>(size_t slot) const
{
	assert(slot < MAX_ATTRIBS);
	GenericValue attrib = attribs[slot];
	assert(attrib.count == 2);

	return { attrib.vals[0], attrib.vals[1] };
}

template <>
inline math::vec3 VsInput::Get<math::vec3>(size_t slot) const
{
	assert(slot < MAX_ATTRIBS);
	GenericValue attrib = attribs[slot];
	assert(attrib.count == 3);

	return { attrib.vals[0], attrib.vals[1], attrib.vals[2] };
}

template <>
inline math::vec4 VsInput::Get<math::vec4>(size_t slot) const
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

	// TODO: not the best solution
	const size_t Size() const
	{
		return (size_t)used;
	}

	template <typename T>
	T getVarying(uint32_t loc) const;

	math::vec4 Position = {};

//private:
	int used = 0;
	static constexpr size_t MAX_VARYINGS = 8;
	std::array<GenericValue, MAX_VARYINGS> varyings = {0};
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

template <typename ShaderProgram>
struct Renderer
{
	Renderer(rnd::framebuffer& fb)
		:
		_fb(fb)
	{
	}

	void BindShaderProgram(const ShaderProgram* program)
	{
		this->program = program;
	}

	int CreateVertexBuffer(const void* data, size_t stride)
	{
		VertexBuffer vbo = { (uint8_t*)data, stride };

		vertexBuffers.push_back(vbo);

		// TODO: ehh?
		return vertexBuffers.size() - 1;
	}

	void BindVertexBuffer(int bufferId)
	{
		// TODO: add state for no bound buffer. (e.g. -1)
		assert(bufferId < vertexBuffers.size());
		boundBuffer = &vertexBuffers[bufferId];
	}

	void SetVertexAttribute(int bufferId, VertexAttrib attrib)
	{
		VertexBuffer& vbo = vertexBuffers[bufferId];
		vbo.attribs.push_back(attrib);
	}

	void Draw(size_t num_vertices)
	{
		assert(boundBuffer);
		const std::vector<VertexAttrib>& attributes = boundBuffer->attribs;

		size_t nTriangles = num_vertices / 3;

		// for each triangle
		for (int i = 0; i < nTriangles; ++i)
		{
			VsInput input0{};
			VsInput input1{};
			VsInput input2{};


			// for each attribute in the vertex
			for (const VertexAttrib& a : attributes)
			{
				uint8_t* ptr0 = boundBuffer->data + boundBuffer->stride * (3 * i) + a.offset;
				uint8_t* ptr1 = ptr0 + boundBuffer->stride;
				uint8_t* ptr2 = ptr1 + boundBuffer->stride;

				GenericValue val0 = ExtractValue(ptr0, a);
				GenericValue val1 = ExtractValue(ptr1, a);
				GenericValue val2 = ExtractValue(ptr2, a);

				input0.Set(a.slot, val0);
				input1.Set(a.slot, val1);
				input2.Set(a.slot, val2);
			}

			VSOutput vsout[3];
			vsout[0] = program->vs(input0);
			vsout[1] = program->vs(input1);
			vsout[2] = program->vs(input2);

			vsout[0].Position = _viewport.transform(perspective_divide( vsout[0].Position ));
			vsout[1].Position = _viewport.transform(perspective_divide( vsout[1].Position ));
			vsout[2].Position = _viewport.transform(perspective_divide( vsout[2].Position ));

			rnd::f32 area = math::det_2d(
				vsout[1].Position - vsout[0].Position,
				vsout[2].Position - vsout[0].Position
			);

			// backface culling
			const rnd::b8 ccw = area < 0.f;
			if (!ccw)
				continue;
			std::swap(vsout[1], vsout[2]);
			area = -area;

			draw_triangle_basic(vsout[0], vsout[1], vsout[2], area);
		}
	}

private:
	void draw_triangle_basic(VSOutput& v0, VSOutput& v1, VSOutput& v2, rnd::f32 area)
	{
		rnd::f32 rcp_area = 1.f / area;

		rnd::i32 xmin = (rnd::i32)util::min3(v0.Position.x, v1.Position.x, v2.Position.x);
		rnd::i32 xmax = (rnd::i32)util::max3(v0.Position.x, v1.Position.x, v2.Position.x);
		rnd::i32 ymin = (rnd::i32)util::min3(v0.Position.y, v1.Position.y, v2.Position.y);
		rnd::i32 ymax = (rnd::i32)util::max3(v0.Position.y, v1.Position.y, v2.Position.y);

		// Clamp to viewport bounds.
		xmin = std::clamp(xmin, _viewport.xmin, _viewport.xmax - 1);
		xmax = std::clamp(xmax, _viewport.xmin, _viewport.xmax - 1);
		ymin = std::clamp(ymin, _viewport.ymin, _viewport.ymax - 1);
		ymax = std::clamp(ymax, _viewport.ymin, _viewport.ymax - 1);

		for (rnd::i32 y = ymin; y <= ymax; ++y)
		{
			for (rnd::i32 x = xmin; x <= xmax; ++x)
			{
				math::vec4 p{ x + 0.5f, y + 0.5f, 0.f, 0.f };

				float det01p = math::det_2d(v1.Position - v0.Position, p - v0.Position);
				float det12p = math::det_2d(v2.Position - v1.Position, p - v1.Position);
				float det20p = math::det_2d(v0.Position - v2.Position, p - v2.Position);

				if (det01p < 0.f || det12p < 0.f || det20p < 0.f)
					continue;

				float alpha = det12p * rcp_area;
				float beta = det20p * rcp_area;
				float gamma = det01p * rcp_area;

				VSOutput interpolated;
				//interpolated.varyings.resize(v0.varyings.size());
				interpolated.Position = v0.Position * alpha + v1.Position * beta + v2.Position * gamma;

				// for each vertex attribute
				for (int j = 0; j < v0.Size(); ++j)
				{
					const GenericValue& a0 = v0.varyings[j];
					const GenericValue& a1 = v1.varyings[j];
					const GenericValue& a2 = v2.varyings[j];

					GenericValue interp = InterpolateAttrib(a0, a1, a2, alpha, beta, gamma);
					interpolated.varyings[j] = interp;
				}

				math::vec4 color = program->fs(interpolated);

				_fb.put_pixel((int)x, (int)y, rnd::to_color(color));
			}
		}
	}
private:
	inline static math::vec4 perspective_divide(math::vec4 v)
	{
		v.w = 1.f / v.w;
		v.x *= v.w;
		v.y *= v.w;
		v.z *= v.w;
		return v;
	}
	GenericValue InterpolateAttrib(const GenericValue& a0, const GenericValue& a1, const GenericValue& a2, float alpha, float beta, float gamma)
	{
		size_t cnt = a0.count;

		GenericValue result = {0};
		result.count = cnt;

		for (size_t i = 0; i < cnt; ++i)
		{
			result.vals[i] += a0.vals[i] * alpha + a1.vals[i] * beta + a2.vals[i] * gamma;
		}
		return result;
	}
private:
	rnd::framebuffer& _fb;

	const VertexBuffer* boundBuffer = nullptr;
	const ShaderProgram* program = nullptr;
	//std::function<VSOutput(const VsInput&)> vertexShader;
	//std::function<math::vec4(const VSOutput&)> fragmentShader;
	std::vector<VertexBuffer> vertexBuffers;

	viewport _viewport = { 0, 0, 800, 600 };

};