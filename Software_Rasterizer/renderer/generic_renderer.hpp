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
#include "generic_value.hpp"
#include "buffers.hpp"
#include "varying.hpp"
#include "handle_manager.hpp"

struct Triangle 
{
	VSOutput v0, v1, v2;
	rnd::f32 area;
};

template <typename ShaderProgram>
struct Renderer
{
	static constexpr size_t MAX_TRIS = 5'000;

	Renderer(rnd::framebuffer& fb)
		:
		_fb(fb)
	{
		binData = (int*)std::malloc(NUM_TX * NUM_TY * MAX_TRI_PER_TILE * sizeof(int));
		binCount = (int*)std::calloc(NUM_TX * NUM_TY, sizeof(int));

		// TODO: maybe not...
		triangles = (Triangle*)std::malloc(MAX_TRIS * sizeof(Triangle));
		usedTriangles = 0;
	}

	void BindShaderProgram(const ShaderProgram* program)
	{
		this->program = program;
	}

	rnd::resource_handle CreateVertexBuffer(const void* data, size_t stride)
	{
		rnd::resource_handle vbo_handle = vertex_buffer_manager.emplace((const rnd::u8*)data, stride);
		return vbo_handle;
	}

	rnd::resource_handle CreateIndexBuffer(const rnd::u16* data, size_t cnt)
	{
		rnd::resource_handle ibo_handle = index_buffer_manager.emplace(data, cnt);
		return ibo_handle;
	}

	void BindIndexBuffer(rnd::resource_handle bufferId)
	{
		boundIndexBuffer = index_buffer_manager.get_ptr(bufferId);
	}

	void BindVertexBuffer(rnd::resource_handle bufferId)
	{
		boundBuffer = vertex_buffer_manager.get_ptr(bufferId);
	}

	void DestroyVertexBuffer(rnd::resource_handle bufferId)
	{
		vertex_buffer_manager.free(bufferId);
	}

	void DestroyIndexBuffer(rnd::resource_handle bufferId)
	{
		index_buffer_manager.free(bufferId);
	}

	void UnbindVertexBuffer(rnd::resource_handle bufferId)
	{
		boundBuffer = nullptr;
	}

	void UnbindIndexBuffer(rnd::resource_handle bufferId)
	{
		boundIndexBuffer = nullptr;
	}

	void SetVertexAttribute(VertexAttrib attrib)
	{
		assert(boundBuffer);
		boundBuffer->add_attrib(attrib);
	}

	void SetViewport(math::vec2i start, math::vec2i end)
	{
		_viewport = { start.x, start.y, end.x, end.y };
	}

	void DrawIndexedBin(size_t num_indices)
	{
		assert(boundBuffer);
		assert(boundIndexBuffer);
		usedTriangles = 0;

		size_t nTriangles = num_indices / 3;

		// for each triangle
		for (int i = 0; i < nTriangles; ++i)
		{
			VSInput input0{};
			VSInput input1{};
			VSInput input2{};

			rnd::u16 idx0 = boundIndexBuffer->data[i * 3 + 0];
			rnd::u16 idx1 = boundIndexBuffer->data[i * 3 + 1];
			rnd::u16 idx2 = boundIndexBuffer->data[i * 3 + 2];

			// for each attribute in the vertex
			for (const VertexAttrib& a : boundBuffer->get_attribs())
			{
				const uint8_t* ptr0 = boundBuffer->get_data() + boundBuffer->get_stride() * idx0 + a.offset;
				const uint8_t* ptr1 = boundBuffer->get_data() + boundBuffer->get_stride() * idx1 + a.offset;
				const uint8_t* ptr2 = boundBuffer->get_data() + boundBuffer->get_stride() * idx2 + a.offset;

				GenericValue val0 = extract_vertex_attribute(ptr0, a);
				GenericValue val1 = extract_vertex_attribute(ptr1, a);
				GenericValue val2 = extract_vertex_attribute(ptr2, a);

				input0.Set(a.slot, val0);
				input1.Set(a.slot, val1);
				input2.Set(a.slot, val2);
			}
			VSOutput vsout[3];

			// vertex shader
			vsout[0] = program->vs(input0);
			vsout[1] = program->vs(input1);
			vsout[2] = program->vs(input2);

			// perspective division and viewport trasnform
			vsout[0].Position = _viewport.transform(perspective_divide(vsout[0].Position));
			vsout[1].Position = _viewport.transform(perspective_divide(vsout[1].Position));
			vsout[2].Position = _viewport.transform(perspective_divide(vsout[2].Position));

			// perspective correction
			std::size_t sz = vsout->Size();
			for (int j = 0; j < sz; ++j)
			{
				GenericValue& gv0 = vsout[0].varyings[j];
				GenericValue& gv1 = vsout[1].varyings[j];
				GenericValue& gv2 = vsout[2].varyings[j];

				const std::size_t sz = gv0.count;
				for (int j = 0; j < sz; ++j)
				{
					gv0.vals[j] = gv0.vals[j] * vsout[0].Position.w;
					gv1.vals[j] = gv1.vals[j] * vsout[1].Position.w;
					gv2.vals[j] = gv2.vals[j] * vsout[2].Position.w;
				}
			}

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

			triangles[usedTriangles++] = Triangle{ vsout[0], vsout[1], vsout[2], area };
		}

		// setup triangles for tiled rendering
		setupTriangles();

		// rasterize tiles
		for (int ty = 0; ty < NUM_TY; ++ty)
		{
			for (int tx = 0; tx < NUM_TX; ++tx)
			{
				if (binCount[ty * NUM_TX + tx] > 0)
					rasterizeTile(tx, ty);
			}
		}
	}

	void DrawIndexed(size_t num_indices)
	{
		assert(boundBuffer);
		assert(boundIndexBuffer);

		size_t nTriangles = num_indices / 3;

		// for each triangle
		for (int i = 0; i < nTriangles; ++i)
		{
			VSInput input0{};
			VSInput input1{};
			VSInput input2{};

			rnd::u16 idx0 = boundIndexBuffer->data[i * 3 + 0];
			rnd::u16 idx1 = boundIndexBuffer->data[i * 3 + 1];
			rnd::u16 idx2 = boundIndexBuffer->data[i * 3 + 2];

			// for each attribute in the vertex
			for (const VertexAttrib& a : boundBuffer->get_attribs())
			{
				const uint8_t* ptr0 = boundBuffer->get_data() + boundBuffer->get_stride() * idx0 + a.offset;
				const uint8_t* ptr1 = boundBuffer->get_data() + boundBuffer->get_stride() * idx1 + a.offset;
				const uint8_t* ptr2 = boundBuffer->get_data() + boundBuffer->get_stride() * idx2 + a.offset;

				GenericValue val0 = extract_vertex_attribute(ptr0, a);
				GenericValue val1 = extract_vertex_attribute(ptr1, a);
				GenericValue val2 = extract_vertex_attribute(ptr2, a);

				input0.Set(a.slot, val0);
				input1.Set(a.slot, val1);
				input2.Set(a.slot, val2);
			}

			VSOutput vsout[3];

			// vertex shader
			vsout[0] = program->vs(input0);
			vsout[1] = program->vs(input1);
			vsout[2] = program->vs(input2);

			// perspective division and viewport trasnform
			vsout[0].Position = _viewport.transform(perspective_divide(vsout[0].Position));
			vsout[1].Position = _viewport.transform(perspective_divide(vsout[1].Position));
			vsout[2].Position = _viewport.transform(perspective_divide(vsout[2].Position));

			// perspective correction
			std::size_t sz = vsout->Size();
			for (int j = 0; j < sz; ++j)
			{
				GenericValue& gv0 = vsout[0].varyings[j];
				GenericValue& gv1 = vsout[1].varyings[j];
				GenericValue& gv2 = vsout[2].varyings[j];

				const std::size_t sz = gv0.count;
				for (int j = 0; j < sz; ++j)
				{
					gv0.vals[j] = gv0.vals[j] * vsout[0].Position.w;
					gv1.vals[j] = gv1.vals[j] * vsout[1].Position.w;
					gv2.vals[j] = gv2.vals[j] * vsout[2].Position.w;
				}
			}

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

			//draw_triangle_basic(vsout[0], vsout[1], vsout[2], area);
			draw_triangle_basic_test(vsout[0], vsout[1], vsout[2], area);
		}

	}

	void Draw(size_t num_vertices)
	{
		assert(boundBuffer);
		const std::vector<VertexAttrib>& attributes = boundBuffer->get_attribs();

		size_t nTriangles = num_vertices / 3;

		// for each triangle
		for (int i = 0; i < nTriangles; ++i)
		{
			VSInput input0{};
			VSInput input1{};
			VSInput input2{};

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
	void draw_triangle_basic_test(VSOutput& v0, VSOutput& v1, VSOutput& v2, rnd::f32 area)
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

				_fb.put_pixel((int)x, (int)y, rnd::green);
			}
		}
	}
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

				float oneOverZ = alpha * v0.Position.w + beta * v1.Position.w + gamma * v2.Position.w;
				float z = 1.f / oneOverZ;

				if (z >= _fb.get_depth(x, y))
					continue;
				_fb.set_depth(x, y, z);

				VSOutput interpolated;
				interpolated.Position = (v0.Position * alpha + v1.Position * beta + v2.Position * gamma) * z;

				// for each vertex attribute
				for (int j = 0; j < v0.Size(); ++j)
				{
					const GenericValue& a0 = v0.varyings[j];
					const GenericValue& a1 = v1.varyings[j];
					const GenericValue& a2 = v2.varyings[j];

					GenericValue interp = Interpolate(a0, a1, a2, alpha, beta, gamma, z);
					interpolated.varyings[j] = interp;
				}

				math::vec4 color = program->fs(interpolated);

				_fb.put_pixel((int)x, (int)y, rnd::to_color(color));
			}
		}
	}
	inline static math::vec4 perspective_divide(math::vec4 v)
	{
		v.w = 1.f / v.w;
		v.x *= v.w;
		v.y *= v.w;
		v.z *= v.w;
		return v;
	}
private:
	// binning
	void rasterizeTile(int tx, int ty)
	{
		int idx = ty * NUM_TX + tx;

		for (int bi = 0; bi < binCount[idx]; ++bi) 
		{
			const Triangle& t = triangles[binData[idx * MAX_TRI_PER_TILE + bi]];
			int startX = tx * TILE_W;
			int startY = ty * TILE_H;
			int endX = startX + TILE_W;
			int endY = startY + TILE_H;

			// calculate the god damn triangle's bounding box here jesus christ.
			rnd::i32 xmin = (rnd::i32)util::min3(t.v0.Position.x, t.v1.Position.x, t.v2.Position.x);
			rnd::i32 xmax = (rnd::i32)util::max3(t.v0.Position.x, t.v1.Position.x, t.v2.Position.x);
			rnd::i32 ymin = (rnd::i32)util::min3(t.v0.Position.y, t.v1.Position.y, t.v2.Position.y);
			rnd::i32 ymax = (rnd::i32)util::max3(t.v0.Position.y, t.v1.Position.y, t.v2.Position.y);

			// Clamp to viewport bounds.
			xmin = std::clamp(xmin, startX, endX);
			xmax = std::clamp(xmax, startX, endX);
			ymin = std::clamp(ymin, startY, endY);
			ymax = std::clamp(ymax, startY, endY);

			for (int y = ymin; y <= ymax; ++y)
			{
				for (int x = xmin; x <= xmax; ++x)
				{
					math::vec4 p{ x + 0.5f, y + 0.5f, 0.f, 0.f };

					float det01p = math::det_2d(t.v1.Position - t.v0.Position, p - t.v0.Position);
					float det12p = math::det_2d(t.v2.Position - t.v1.Position, p - t.v1.Position);
					float det20p = math::det_2d(t.v0.Position - t.v2.Position, p - t.v2.Position);

					if (det01p < 0.f || det12p < 0.f || det20p < 0.f)
						continue;

					_fb.put_pixel((int)x, (int)y, rnd::red);
				}
			}
		}
	}

	void setupTriangles()
	{
		//memset(binData, 0, NUM_TX * NUM_TY * MAX_TRI_PER_TILE * sizeof(int));
		memset(binCount, 0, NUM_TX * NUM_TY * sizeof(int));

		// bin each triangle by it's bounding box
		for (int ti = 0; ti < (int)usedTriangles; ++ti)
		{
			Triangle& t = triangles[ti];
			// bbox
			int minX = std::floor(std::min({ t.v0.Position.x, t.v1.Position.x, t.v2.Position.x }));
			int maxX = std::ceil(std::max({ t.v0.Position.x, t.v1.Position.x, t.v2.Position.x }));
			int minY = std::floor(std::min({ t.v0.Position.y, t.v1.Position.y, t.v2.Position.y }));
			int maxY = std::ceil(std::max({ t.v0.Position.y, t.v1.Position.y, t.v2.Position.y }));

			// tile bounds
			int tx0 = std::max(0, minX / TILE_W);
			int tx1 = std::min(NUM_TX - 1, maxX / TILE_W);
			int ty0 = std::max(0, minY / TILE_H);
			int ty1 = std::min(NUM_TY - 1, maxY / TILE_H);

			for (int ty = ty0; ty <= ty1; ++ty)
			{
				for (int tx = tx0; tx <= tx1; ++tx)
				{
					int idx = ty * NUM_TX + tx;
					int c = binCount[idx]++;
					assert(c < MAX_TRI_PER_TILE);
					binData[idx * MAX_TRI_PER_TILE + c] = ti;	// binData[idx][c] = ti;
				}
			}
		}
	}
private:
	rnd::framebuffer& _fb;

	VertexBuffer* boundBuffer = nullptr;
	IndexBuffer* boundIndexBuffer = nullptr;
	const ShaderProgram* program = nullptr;

	rnd::resource_manager<VertexBuffer, 16> vertex_buffer_manager;
	rnd::resource_manager<IndexBuffer, 16> index_buffer_manager;

	viewport _viewport = { 0 };

	// binning
	static constexpr int W = 800;      
	static constexpr int H = 600;      
	static constexpr int TILE_W = 64;	// tile size
	static constexpr int TILE_H = 128;
	static constexpr int NUM_TX = (W + TILE_W - 1) / TILE_W;
	static constexpr int NUM_TY = (H + TILE_H - 1) / TILE_H;
	static constexpr int MAX_TRI_PER_TILE = 16;

	//std::vector<Triangle> triangles;
	Triangle* triangles = nullptr;
	size_t usedTriangles = 0;
	int* binData = nullptr;		// [NUM_TX * NUM_TY][MAX_TRI_PER_TILE]
	int* binCount = nullptr;	// [NUM_TX * NUM_TY]
};