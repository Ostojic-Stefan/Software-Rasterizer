//#pragma once
//
//#include <Engine/engine.hpp>
//#include <algorithm>
//#include "viewport.hpp"
//
//struct vertex
//{
//public:
//	math::vec4 position;
//	math::vec3 world_position;
//	math::vec2 tex_coord;
//	math::vec3 normal;
//};
//
//struct Triangle
//{
//	vertex v0;
//	vertex v1;
//	vertex v2;
//};
//
//
//template<typename T>
//inline T interpolate(const T& src, const T& dst, float alpha)
//{
//	return src + (dst - src) * alpha;
//}
//
//class Pipeline
//{
//public:
//	Pipeline(rnd::framebuffer& fb) : fb(fb) {}
//
//	void bindModel(const math::mat4& model)
//	{
//		this->model = model;
//	}
//
//	void bindView(const math::mat4& view)
//	{
//		this->view = view;
//	}
//
//	void bindProj(const math::mat4& proj)
//	{
//		this->proj = proj;
//	}
//
//	void bindTexture(gfx::texture* tex)
//	{
//		this->texture = tex;
//	}
//
//	void Draw(const gfx::mesh& mesh)
//	{
//		ProcessVertices(mesh);
//	}
//
//
//private:
//	void ProcessVertices(const gfx::mesh& mesh)
//	{
//		std::vector<vertex> vertices_out;
//
//		for (int i = 0; i < mesh.positions.size(); ++i)
//		{
//			vertex vert = {
//				.position = proj * view * model * math::vec4(mesh.positions[i], 1.0f),
//				.world_position = model * mesh.positions[i],
//				.tex_coord = mesh.tex_coords[i],
//				.normal = mesh.normals[i]
//			};
//
//			vertices_out.emplace_back(vert);
//		}
//
//		AssembleTriangles(vertices_out, mesh.indices);
//	}
//
//	void AssembleTriangles(const std::vector<vertex>& vertices, const std::vector<rnd::u32>& indices)
//	{
//		for (size_t i = 0, end = indices.size() / 3; i < end; i++)
//		{
//			// determine triangle vertices via indexing
//			const auto& v0 = vertices[indices[i * 3]];
//			const auto& v1 = vertices[indices[i * 3 + 1]];
//			const auto& v2 = vertices[indices[i * 3 + 2]];
//
//			ProcessTriangle(v0, v1, v2);
//		}
//	}
//
//	void ProcessTriangle(const vertex& v0, const vertex& v1, const vertex& v2)
//	{
//		Triangle t{ v0, v1, v2 };
//		PostProcessTriangleVertices(t);
//	}
//
//	void PostProcessTriangleVertices(Triangle& triangle)
//	{
//		pstTransform(triangle.v0);
//		pstTransform(triangle.v1);
//		pstTransform(triangle.v2);
//
//		DrawTriangle(triangle);
//	}
//
//	void DrawTriangle(Triangle& triangle)
//	{
//		 vertex& v0 = triangle.v0;
//		 vertex& v1 = triangle.v1;
//		 vertex& v2 = triangle.v2;
//
//		rnd::f32 area = math::det_2d(
//			v1.position - v0.position,
//			v2.position - v0.position
//		);
//
//		// backface culling
//		const rnd::b8 ccw = area < 0.f;
//		if (!ccw)
//			return;
//		std::swap(v1, v2);
//		area = -area;
//
//
//		rnd::i32 xmin = (rnd::i32)util::min3(v0.position.x, v1.position.x, v2.position.x);
//		rnd::i32 xmax = (rnd::i32)util::max3(v0.position.x, v1.position.x, v2.position.x);
//		rnd::i32 ymin = (rnd::i32)util::min3(v0.position.y, v1.position.y, v2.position.y);
//		rnd::i32 ymax = (rnd::i32)util::max3(v0.position.y, v1.position.y, v2.position.y);
//
//		// Clamp to viewport bounds.
//		xmin = std::clamp(xmin, 0, 800 - 1);
//		xmax = std::clamp(xmax, 0, 800 - 1);
//		ymin = std::clamp(ymin, 0, 600 - 1);
//		ymax = std::clamp(ymax, 0, 600 - 1);
//
//		for (rnd::i32 y = ymin; y <= ymax; ++y)
//		{
//			for (rnd::i32 x = xmin; x <= xmax; ++x)
//			{
//				math::vec4 p{ x + 0.5f, y + 0.5f, 0.f, 0.f };
//
//				float det01p = math::det_2d(v1.position - v0.position, p - v0.position);
//				float det12p = math::det_2d(v2.position - v1.position, p - v1.position);
//				float det20p = math::det_2d(v0.position - v2.position, p - v2.position);
//
//				if (det01p < 0.f || det12p < 0.f || det20p < 0.f)
//					continue;
//
//				float alpha = det12p / area;
//				float beta  = det20p / area;
//				float gamma = det01p / area;
//
//				float oneOverZ = alpha * v0.position.w + beta * v1.position.w + gamma * v2.position.w;
//				float z = 1.0f / oneOverZ;
//
//				if (z >= fb.get_depth(x, y)) {
//					continue;
//				}
//				fb.set_depth(x, y, z);
//
//				math::vec2 st = alpha * v0.tex_coord + beta * v1.tex_coord + gamma * v2.tex_coord;
//				st = st * z;
//				st.x = std::clamp(st.x, 0.0f, 1.0f);
//				st.y = std::clamp(st.y, 0.0f, 1.0f);
//
//				math::vec4 sample = texture->sample(st.x, st.y);
//
//				fb.put_pixel(x, y, rnd::to_color(sample));
//			}
//		}
//	}
//
//	void pstTransform(vertex& v)
//	{
//		const float zInv = 1.0f / v.position.w;
//
//		v.position = zInv * v.position;
//		v.normal = zInv * v.normal;
//		v.tex_coord = zInv * v.tex_coord;
//		v.world_position = zInv * v.world_position;
//
//		v.position.x = (v.position.x + 1.0f) * 400;
//		v.position.y = (-v.position.y + 1.0f) * 300;
//
//		//v.position.z = zInv;
//		v.position.w = zInv;
//	}
//
//
//	//inline static math::vec4 perspective_divide(math::vec4 v)
//	//{
//	//	v.w = 1.f / v.w;
//	//	v.x *= v.w;
//	//	v.y *= v.w;
//	//	v.z *= v.w;
//	//	return v;
//	//}
//
//private:
//	rnd::framebuffer& fb;
//	math::mat4 model;
//	math::mat4 view;
//	math::mat4 proj;
//	gfx::texture* texture;
//};
//
