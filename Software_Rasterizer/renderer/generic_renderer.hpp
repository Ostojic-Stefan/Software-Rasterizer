#pragma once

#include <vector>
#include <cassert>
#include <cstring>
#include <array>
#include <type_traits>
#include <functional>
#include <span>
#include <memory>

#include "types.hpp"
#include "math/vector.hpp"

#include "viewport.hpp"
#include "generic_value.hpp"
#include "buffers.hpp"
#include "varying.hpp"
#include "handle_manager.hpp"
#include "frame_buffer.hpp"

#include "SimpleThreadPool.h"

struct Triangle
{
	VSOutput v0, v1, v2;
	rnd::f32 area;

	bool culled = false;
};

struct TileRasterizerFunctor
{
	void operator()(int tileStartX, int tileStartY, int tileEndX, int tileEndY, const Triangle& t, rnd::color* color_buffer, rnd::f32* depth_buffer, rnd::u32 fb_width, rnd::color col)
	{
		// calculate the god damn triangle's bounding box here jesus christ.
		rnd::i32 xmin = (rnd::i32)util::min3(t.v0.Position.x, t.v1.Position.x, t.v2.Position.x);
		rnd::i32 xmax = (rnd::i32)util::max3(t.v0.Position.x, t.v1.Position.x, t.v2.Position.x);
		rnd::i32 ymin = (rnd::i32)util::min3(t.v0.Position.y, t.v1.Position.y, t.v2.Position.y);
		rnd::i32 ymax = (rnd::i32)util::max3(t.v0.Position.y, t.v1.Position.y, t.v2.Position.y);

		// Clamp to the tile bounds.
		xmin = std::clamp(xmin, tileStartX, tileEndX - 1);
		xmax = std::clamp(xmax, tileStartX, tileEndX - 1);
		ymin = std::clamp(ymin, tileStartY, tileEndY - 1);
		ymax = std::clamp(ymax, tileStartY, tileEndY - 1);

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

				color_buffer[y * fb_width + x] = col;
			}
		}
	}
};

template <typename ShaderProgram>
struct Renderer
{
	static constexpr size_t MAX_TRIS = 15'000;
	static constexpr size_t nThreads = 4;

	Renderer(rnd::framebuffer& fb)
		:
		_fb(fb),
		_threadPool(nThreads)
	{
		binData = (int*)std::malloc(NUM_TX * NUM_TY * MAX_TRI_PER_TILE * sizeof(int));
		//binCount = (std::atomic<int>*)std::calloc(NUM_TX * NUM_TY, sizeof(int));
		binCount = std::make_unique<std::atomic<int>[]>(NUM_TX * NUM_TY);

		// TODO: maybe not...
		triangles = (Triangle*)std::malloc(MAX_TRIS * sizeof(Triangle));

		//activeTiles.reserve(NUM_TX * NUM_TY);
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

		for (std::atomic<int>* ptr = binCount.get(), *end = binCount.get() + (NUM_TX * NUM_TY); ptr != end; ++ptr)
			ptr->store(0, std::memory_order_relaxed);

		const size_t nTriangles = num_indices / 3;
		const size_t triPerThread = (nTriangles + (nThreads - 1)) / nThreads;

		for (int i = 0; i < nThreads; ++i)
		{
			const int start = i * triPerThread;
			const int end = std::min(start + triPerThread, nTriangles);

			_threadPool.enqueue([this, start, end] {
				processTriangleVertices(start, end, triangles);
			});
		}

		_threadPool.waitAll();

#if 1

		constexpr size_t TILES_PER_THREAD = 8u;

		// rasterize tiles
		constexpr size_t totalTiles = NUM_TX * NUM_TY;
		constexpr size_t numGroups = (totalTiles + TILES_PER_THREAD - 1) / TILES_PER_THREAD;

		for (size_t group = 0; group < numGroups; ++group)
		{
			const size_t startIdx = group * TILES_PER_THREAD;
			const size_t endIdx = std::min(startIdx + TILES_PER_THREAD, totalTiles);

			_threadPool.enqueue([this, startIdx, endIdx] {
				for (size_t idx = startIdx; idx < endIdx; ++idx)
				{
					// skip empty bins
					if (binCount[idx].load(std::memory_order_relaxed) == 0)
						continue;

					const int ty = idx / NUM_TX;
					const int tx = idx % NUM_TX;

					const int tileStartX = tx * TILE_W;
					const int tileStartY = ty * TILE_H;
					const int tileEndX = std::min(tileStartX + TILE_W, W);
					const int tileEndY = std::min(tileStartY + TILE_H, H);

					for (int bi = 0, n = binCount[idx].load(); bi < n; ++bi)
					{
						TileRasterizerFunctor()(
							tileStartX, tileStartY,
							tileEndX, tileEndY,
							triangles[binData[idx * MAX_TRI_PER_TILE + bi]],
							_fb.color_buffer.get(),
							_fb.depth_buffer.get(),
							_fb.get_width(),
							rnd::red
						);
					}
				}
			});
		}
		_threadPool.waitAll();

#else
		// old
		for (int ty = 0; ty < NUM_TY; ++ty)
		{
			for (int tx = 0; tx < NUM_TX; ++tx)
			{
				int idx = ty * NUM_TX + tx;
				if (binCount[idx] > 0)
				{
					int tileStartX = tx * TILE_W;
					int tileStartY = ty * TILE_H;
					int tileEndX = std::min(tileStartX + TILE_W, W);
					int tileEndY = std::min(tileStartY + TILE_H, H);

					std::vector<Triangle> tris;
					tris.reserve(binCount[idx]);
					for (int bi = 0; bi < binCount[idx].load(); ++bi)
					{
						const Triangle& t = triangles[binData[idx * MAX_TRI_PER_TILE + bi]];
						tris.push_back(t);
					}

					TileRasterizerFunctor()(tileStartX, tileStartY, tileEndX, tileEndY, std::move(tris), _fb.color_buffer.get(), _fb.depth_buffer.get(), _fb.get_width());
				}
			}
		}

#endif
		
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

			const rnd::u16 idx0 = boundIndexBuffer->data[i * 3 + 0];
			const rnd::u16 idx1 = boundIndexBuffer->data[i * 3 + 1];
			const rnd::u16 idx2 = boundIndexBuffer->data[i * 3 + 2];

			// for each attribute in the vertex
			for (const VertexAttrib& a : boundBuffer->get_attribs())
			{
				const uint8_t* ptr0 = boundBuffer->get_data() + boundBuffer->get_stride() * idx0 + a.offset;
				const uint8_t* ptr1 = boundBuffer->get_data() + boundBuffer->get_stride() * idx1 + a.offset;
				const uint8_t* ptr2 = boundBuffer->get_data() + boundBuffer->get_stride() * idx2 + a.offset;

				const GenericValue val0 = extract_vertex_attribute(ptr0, a);
				const GenericValue val1 = extract_vertex_attribute(ptr1, a);
				const GenericValue val2 = extract_vertex_attribute(ptr2, a);

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

	// instance data:
	// 1. boundIndexBuffer
	// 2. boundBuffer
	// 3. shaderProgram
	// 4. viewport
	void processTriangleVertices(int startRange, int endRange, Triangle* out)
	{
		for (int i = startRange; i < endRange; ++i)
		{
			VSInput input0{};
			VSInput input1{};
			VSInput input2{};

			const rnd::u16 idx0 = boundIndexBuffer->data[i * 3 + 0];
			const rnd::u16 idx1 = boundIndexBuffer->data[i * 3 + 1];
			const rnd::u16 idx2 = boundIndexBuffer->data[i * 3 + 2];

			for (const VertexAttrib& a : boundBuffer->get_attribs())
			{
				const uint8_t* ptr0 = boundBuffer->get_data() + boundBuffer->get_stride() * idx0 + a.offset;
				const uint8_t* ptr1 = boundBuffer->get_data() + boundBuffer->get_stride() * idx1 + a.offset;
				const uint8_t* ptr2 = boundBuffer->get_data() + boundBuffer->get_stride() * idx2 + a.offset;

				const GenericValue val0 = extract_vertex_attribute(ptr0, a);
				const GenericValue val1 = extract_vertex_attribute(ptr1, a);
				const GenericValue val2 = extract_vertex_attribute(ptr2, a);

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
			const std::size_t sz = vsout->Size();
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

			bool culled = false;

			// backface culling
			const rnd::b8 ccw = area < 0.f;
			if (!ccw)
				culled = true;

			std::swap(vsout[1], vsout[2]);
			area = -area;

			out[i] = Triangle{ vsout[0], vsout[1], vsout[2], area, culled };

		}

		setupTrianglesRange(startRange, endRange, out);
	}

	//void rasterizeTile(rnd::framebuffer& fb, int tileStartX, int tileStartY, int tileEndX, int tileEndY, std::vector<Triangle> triangles)
	//{
	//	for (const auto& t : triangles)
	//	{
	//		// calculate the god damn triangle's bounding box here jesus christ.
	//		rnd::i32 xmin = (rnd::i32)util::min3(t.v0.Position.x, t.v1.Position.x, t.v2.Position.x);
	//		rnd::i32 xmax = (rnd::i32)util::max3(t.v0.Position.x, t.v1.Position.x, t.v2.Position.x);
	//		rnd::i32 ymin = (rnd::i32)util::min3(t.v0.Position.y, t.v1.Position.y, t.v2.Position.y);
	//		rnd::i32 ymax = (rnd::i32)util::max3(t.v0.Position.y, t.v1.Position.y, t.v2.Position.y);

	//		// Clamp to the tile bounds.
	//		xmin = std::clamp(xmin, tileStartX, tileEndX - 1);
	//		xmax = std::clamp(xmax, tileStartX, tileEndX - 1);
	//		ymin = std::clamp(ymin, tileStartY, tileEndY - 1);
	//		ymax = std::clamp(ymax, tileStartY, tileEndY - 1);

	//		for (int y = ymin; y <= ymax; ++y)
	//		{
	//			for (int x = xmin; x <= xmax; ++x)
	//			{
	//				math::vec4 p{ x + 0.5f, y + 0.5f, 0.f, 0.f };

	//				float det01p = math::det_2d(t.v1.Position - t.v0.Position, p - t.v0.Position);
	//				float det12p = math::det_2d(t.v2.Position - t.v1.Position, p - t.v1.Position);
	//				float det20p = math::det_2d(t.v0.Position - t.v2.Position, p - t.v2.Position);

	//				if (det01p < 0.f || det12p < 0.f || det20p < 0.f)
	//					continue;

	//				fb.put_pixel((int)x, (int)y, rnd::red);
	//			}
	//		}
	//	}

	// binning
	//void rasterizeTile(int tx, int ty, int idx)
	//{
	//	for (int bi = 0; bi < binCount[idx]; ++bi) 
	//	{
	//		const Triangle& t = triangles[binData[idx * MAX_TRI_PER_TILE + bi]];
	//		int tileStartX = tx * TILE_W;
	//		int tileStartY = ty * TILE_H;
	//		int tileEndX = std::min(tileStartX + TILE_W, W);
	//		int tileEndY = std::min(tileStartY + TILE_H, H);

	//		// calculate the god damn triangle's bounding box here jesus christ.
	//		rnd::i32 xmin = (rnd::i32)util::min3(t.v0.Position.x, t.v1.Position.x, t.v2.Position.x);
	//		rnd::i32 xmax = (rnd::i32)util::max3(t.v0.Position.x, t.v1.Position.x, t.v2.Position.x);
	//		rnd::i32 ymin = (rnd::i32)util::min3(t.v0.Position.y, t.v1.Position.y, t.v2.Position.y);
	//		rnd::i32 ymax = (rnd::i32)util::max3(t.v0.Position.y, t.v1.Position.y, t.v2.Position.y);

	//		// Clamp to the tile bounds.
	//		xmin = std::clamp(xmin, tileStartX, tileEndX - 1);
	//		xmax = std::clamp(xmax, tileStartX, tileEndX - 1);
	//		ymin = std::clamp(ymin, tileStartY, tileEndY - 1);
	//		ymax = std::clamp(ymax, tileStartY, tileEndY - 1);

	//		for (int y = ymin; y <= ymax; ++y)
	//		{
	//			for (int x = xmin; x <= xmax; ++x)
	//			{
	//				math::vec4 p{ x + 0.5f, y + 0.5f, 0.f, 0.f };

	//				float det01p = math::det_2d(t.v1.Position - t.v0.Position, p - t.v0.Position);
	//				float det12p = math::det_2d(t.v2.Position - t.v1.Position, p - t.v1.Position);
	//				float det20p = math::det_2d(t.v0.Position - t.v2.Position, p - t.v2.Position);

	//				if (det01p < 0.f || det12p < 0.f || det20p < 0.f)
	//					continue;

	//				_fb.put_pixel((int)x, (int)y, rnd::red);
	//			}
	//		}
	//	}
	//}

	void setupTrianglesRange(int start, int end, const Triangle* triangles)
	{
		for (int ti = start; ti < end; ++ti)
		{
			const Triangle& t = triangles[ti];

			if (t.culled)
				continue;

			// bbox
			const int minX = std::floor(std::min({ t.v0.Position.x, t.v1.Position.x, t.v2.Position.x }));
			const int maxX = std::ceil(std::max({ t.v0.Position.x, t.v1.Position.x, t.v2.Position.x }));
			const int minY = std::floor(std::min({ t.v0.Position.y, t.v1.Position.y, t.v2.Position.y }));
			const int maxY = std::ceil(std::max({ t.v0.Position.y, t.v1.Position.y, t.v2.Position.y }));

			// tile bounds
			const int tx0 = std::max(0, minX / TILE_W);
			const int tx1 = std::min(NUM_TX - 1, maxX / TILE_W);
			const int ty0 = std::max(0, minY / TILE_H);
			const int ty1 = std::min(NUM_TY - 1, maxY / TILE_H);

			for (int ty = ty0; ty <= ty1; ++ty)
			{
				for (int tx = tx0; tx <= tx1; ++tx)
				{
					const int idx = ty * NUM_TX + tx;

					//int c = binCount[idx]++;
					const int c = binCount[idx].fetch_add(1, std::memory_order_relaxed);

					assert(c < MAX_TRI_PER_TILE);
					binData[idx * MAX_TRI_PER_TILE + c] = ti;	// binData[idx][c] = ti;
				}
			}
		}
	}

	void setupTriangles(int nTriangles)
	{
		// bin each triangle by it's bounding box
		for (int ti = 0; ti < nTriangles; ++ti)
		{
			Triangle& t = triangles[ti];

			if (t.culled)
				continue;

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

					int c = binCount[idx].fetch_add(1, std::memory_order_relaxed);
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
	static constexpr int TILE_W = 128;	// tile size
	static constexpr int TILE_H = 64;
	static constexpr int NUM_TX = (W + TILE_W - 1) / TILE_W;  // (800+63)/64 = 13
	static constexpr int NUM_TY = (H + TILE_H - 1) / TILE_H;  // (600+63)/64 = 10
	static constexpr int MAX_TRI_PER_TILE = 10000;
	// std::vector<int> activeTiles;

	//std::vector<Triangle> triangles;
	Triangle* triangles = nullptr;
	int* binData = nullptr;		// [NUM_TX * NUM_TY][MAX_TRI_PER_TILE]
	//std::atomic<int>* binCount = nullptr;	// [NUM_TX * NUM_TY]
	std::unique_ptr<std::atomic<int>[]> binCount;
	ThreadPool _threadPool;
};