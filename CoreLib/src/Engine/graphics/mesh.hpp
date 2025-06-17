#pragma once

#include "pch.h"
#include "math/vector.hpp"
#include "math/util.hpp"
#include <iostream>

namespace gfx
{
	struct mesh
	{
		std::vector<math::vec3> positions;
		std::vector<math::vec2> tex_coords;
		std::vector<math::vec3> normals;

		std::vector<math::vec4> colors;
		std::vector<rnd::u16>	indices;
	};

	static inline mesh get_tessellated_plane_mesh(unsigned int tessellationX, unsigned int tessellationY)
	{
		std::vector<math::vec3> positions;
		std::vector<math::vec2> tex_coords;
		std::vector<math::vec3> normals;
		std::vector<math::vec4> colors;
		std::vector<rnd::u16> indices;

		// The plane extends from -0.5 to 0.5 in both X and Y.
		// Compute the size of each cell along x and y.
		float stepX = 1.0f / tessellationX;
		float stepY = 1.0f / tessellationY;

		// Generate vertices:
		// There will be (tessellationX + 1) vertices in the x-direction
		// and (tessellationY + 1) vertices in the y-direction.
		for (unsigned int j = 0; j <= tessellationY; ++j) {
			for (unsigned int i = 0; i <= tessellationX; ++i) {
				// Position: interpolate from -0.5 to 0.5
				float x = -0.5f + i * stepX;
				float y = -0.5f + j * stepY;
				positions.push_back({ x, y, 0.0f });

				// Texture coordinates from 0 to 1 across the plane.
				tex_coords.push_back({ static_cast<float>(i) / tessellationX,
										 static_cast<float>(j) / tessellationY });

				// Normal facing forward (for a flat plane in the XY plane, Z is up)
				normals.push_back({ 0.0f, 0.0f, 1.0f });

				// For colors, we can create a gradient or simply use a constant color.
				// Here, we blend using the texture coordinates.
				colors.push_back({ static_cast<float>(i) / tessellationX,
								   static_cast<float>(j) / tessellationY,
								   0.0f, 1.0f });
			}
		}

		// Generate indices for triangles.
		// Each quad (cell) will be divided into two triangles.
		for (unsigned int j = 0; j < tessellationY; ++j) {
			for (unsigned int i = 0; i < tessellationX; ++i) {
				// Calculate the indices of the quad's corners.
				unsigned int topLeft = j * (tessellationX + 1) + i;
				unsigned int topRight = topLeft + 1;
				unsigned int bottomLeft = (j + 1) * (tessellationX + 1) + i;
				unsigned int bottomRight = bottomLeft + 1;

				// First triangle (top-left, bottom-left, top-right)
				indices.push_back(topLeft);
				indices.push_back(bottomLeft);
				indices.push_back(topRight);

				// Second triangle (top-right, bottom-left, bottomRight)
				indices.push_back(topRight);
				indices.push_back(bottomLeft);
				indices.push_back(bottomRight);
			}
		}

		return { positions, tex_coords, normals, colors, indices };
	}

	static inline mesh get_triangle_mesh()
	{
		static const std::vector<math::vec3> positions = {
			{ -0.3f, -0.3f, 0.0f },
			{ 0.3f, -0.3f, 0.0f },
			{ 0.0f, 0.3f, 0.0f },
		};

		static const std::vector<math::vec4> colors = {
			{ 1.0f, 0.0f, 0.0f, 1.0f },
			{ 0.0f, 1.0f, 0.0f, 1.0f },
			{ 0.0f, 0.0f, 1.0f, 1.0f },
		};

		static const std::vector<math::vec2> tex_coords = {
			{ 0, 0 },
			{ 1, 0 },
			{ 1, 1 },
		};

		static const std::vector<math::vec3> normals = {
			{ 0, 0, 1 },
			{ 0, 0, 1 },
			{ 0, 0, 1 },
		};

		static const std::vector<rnd::u16> indices = {
			0,1,2
		};

		return { positions, tex_coords, normals, colors, indices };
	}

	static inline mesh get_plane_mesh()
	{
		static const std::vector<math::vec3> positions = {
			{ -0.5f, -0.5f, 0.0f },	// bottom-left
			{  0.5f, -0.5f, 0.0f },	// bottom-right
			{ -0.5f,  0.5f, 0.0f },	// top-left 
			{  0.5f,  0.5f, 0.0f },	// top-right
		};

		static const std::vector<math::vec4> colors = {
			{ 0.f, 0.f, 0.f, 1.f },
			{ 0.f, 1.f, 0.f, 1.f },
			{ 1.f, 0.f, 0.f, 1.f },
			{ 1.f, 1.f, 0.f, 1.f },
		};

		static const std::vector<math::vec2> tex_coords = {
			{ 0, 0 },
			{ 1, 0 },
			{ 0, 1 },
			{ 1, 1 },
		};

		static const std::vector<math::vec3> normals = {
			{ 0, 0, 1 },
			{ 0, 0, 1 },
			{ 0, 0, 1 },
			{ 0, 0, 1 },
		};

		static const std::vector<rnd::u16> indices = {
			0,1,2,
			1,3,2
		};

		return { positions, tex_coords, normals, colors, indices };
	}


	static inline mesh get_cube_mesh(rnd::f32 size = 0.25f)
	{
		static std::vector<math::vec3> positions = {
			// -X face
			{-size, -size, -size},
			{-size,  size, -size},
			{-size, -size,  size},
			{-size,  size,  size},

			// +X face
			{ size, -size, -size},
			{ size,  size, -size},
			{ size, -size,  size},
			{ size,  size,  size},

			// -Y face
			{-size, -size, -size},
			{ size, -size, -size},
			{-size, -size,  size},
			{ size, -size,  size},

			// +Y face
			{-size,  size, -size},
			{ size,  size, -size},
			{-size,  size,  size},
			{ size,  size,  size},

			// -Z face
			{-size, -size, -size},
			{ size, -size, -size},
			{-size,  size, -size},
			{ size,  size, -size},

			// +Z face
			{-size, -size,  size},
			{ size, -size,  size},
			{-size,  size,  size},
			{ size,  size,  size},
		};
		
		static std::vector<rnd::u16> indices = {
			// -X face
			0,  2,  1,
			1,  2,  3,

			// +X face
			4,  5,  6,
			6,  5,  7,

			// -Y face
			8,  9, 10,
			10,  9, 11,

			// +Y face
			12, 14, 13,
			14, 15, 13,

			// -Z face
			16, 18, 17,
			17, 18, 19,

			// +Z face
			20, 21, 22,
			21, 23, 22,
		};

		static std::vector<math::vec4> colors = {
			// -X face
			{0.f, 1.f, 1.f, 1.f},
			{0.f, 1.f, 1.f, 1.f},
			{0.f, 1.f, 1.f, 1.f},
			{0.f, 1.f, 1.f, 1.f},

			// +X face
			{1.f, 0.f, 0.f, 1.f},
			{1.f, 0.f, 0.f, 1.f},
			{1.f, 0.f, 0.f, 1.f},
			{1.f, 0.f, 0.f, 1.f},

			// -Y face
			{1.f, 0.f, 1.f, 1.f},
			{1.f, 0.f, 1.f, 1.f},
			{1.f, 0.f, 1.f, 1.f},
			{1.f, 0.f, 1.f, 1.f},

			// +Y face
			{0.f, 1.f, 0.f, 1.f},
			{0.f, 1.f, 0.f, 1.f},
			{0.f, 1.f, 0.f, 1.f},
			{0.f, 1.f, 0.f, 1.f},

			// -Z face
			{1.f, 1.f, 0.f, 1.f},
			{1.f, 1.f, 0.f, 1.f},
			{1.f, 1.f, 0.f, 1.f},
			{1.f, 1.f, 0.f, 1.f},

			// +Z face
			{0.f, 0.f, 1.f, 1.f},
			{0.f, 0.f, 1.f, 1.f},
			{0.f, 0.f, 1.f, 1.f},
			{0.f, 0.f, 1.f, 1.f},
		};

		static std::vector<math::vec3> normals = {
			// -X face
			{-1.f,  0.f,  0.f },
			{-1.f,  0.f,  0.f },
			{-1.f,  0.f,  0.f },
			{-1.f,  0.f,  0.f },

			// +X face
			{ 1.f,  0.f,  0.f },
			{ 1.f,  0.f,  0.f },
			{ 1.f,  0.f,  0.f },
			{ 1.f,  0.f,  0.f },

			// -Y face
			{ 0.f, -1.f,  0.f },
			{ 0.f, -1.f,  0.f },
			{ 0.f, -1.f,  0.f },
			{ 0.f, -1.f,  0.f },

			// +Y face
			{ 0.f,  1.f,  0.f },
			{ 0.f,  1.f,  0.f },
			{ 0.f,  1.f,  0.f },
			{ 0.f,  1.f,  0.f },

			// -Z face
			{ 0.f,  0.f, -1.f },
			{ 0.f,  0.f, -1.f },
			{ 0.f,  0.f, -1.f },
			{ 0.f,  0.f, -1.f },

			// +Z face
			{ 0.f,  0.f,  1.f },
			{ 0.f,  0.f,  1.f },
			{ 0.f,  0.f,  1.f },
			{ 0.f,  0.f,  1.f },
		};

		static std::vector<math::vec2> tex_coords = {
			// -X face
			{0.f, 0.f},
			{1.f, 0.f},
			{0.f, 1.f},
			{1.f, 1.f},

			// +X face
			{0.f, 0.f},
			{1.f, 0.f},
			{0.f, 1.f},
			{1.f, 1.f},

			// -Y face
			{0.f, 0.f},
			{1.f, 0.f},
			{0.f, 1.f},
			{1.f, 1.f},

			// +Y face
			{0.f, 0.f},
			{1.f, 0.f},
			{0.f, 1.f},
			{1.f, 1.f},

			// -Z face
			{0.f, 0.f},
			{1.f, 0.f},
			{0.f, 1.f},
			{1.f, 1.f},

			// +Z face
			{0.f, 0.f},
			{1.f, 0.f},
			{0.f, 1.f},
			{1.f, 1.f},
		};


		return { positions, tex_coords, normals, colors, indices };
	}
	
	static inline mesh get_sphere_mesh(rnd::i32 stack_count, rnd::i32 sector_count, rnd::f32 radius = 1.f)
	{
		mesh res;

		rnd::f32 lengthInv = 1.f / radius;

		for (rnd::i32 i = 0; i <= stack_count; ++i)
		{
			rnd::f32 stack_angle = (math::pi32 / 2.f) - (i * math::pi32 / stack_count);

			for (rnd::i32 j = 0; j <= sector_count; ++j)
			{
				rnd::f32 sector_angle = (j * 2 * math::pi32) / sector_count;
				rnd::f32 x = radius * std::cos(stack_angle) * std::sin(sector_angle);
				rnd::f32 y = radius * std::sin(stack_angle);
				rnd::f32 z = radius * std::cos(stack_angle) * std::cos(sector_angle);

				res.positions.push_back({ x,y,z });

				res.colors.push_back(math::vec4({ x,y,z, 1.0f }));
				// normalized vertex normal
				float nx = x * lengthInv;
				float ny = y * lengthInv;
				float nz = z * lengthInv;
				res.normals.push_back({ nx, ny, nz });

				res.tex_coords.push_back(math::vec2({ x, y }));
			}
		}

		for (rnd::i32 i = 0; i < stack_count; ++i)
		{
			rnd::i32 k1 = i * (sector_count + 1);
			rnd::i32 k2 = k1 + sector_count + 1;

			for (rnd::i32 j = 0; j < sector_count; ++j, ++k1, ++k2)
			{
				if (i != 0)
				{
					res.indices.push_back(k1 + 1);
					res.indices.push_back(k1);
					res.indices.push_back(k2);
				}

				if (i != (stack_count - 1))
				{
					res.indices.push_back(k1 + 1);
					res.indices.push_back(k2);
					res.indices.push_back(k2 + 1);
				}
			}
		}
		return res;
	}

	//static inline mesh get_tetrahedron_mesh()
	//{
	//	mesh result;

	//	rnd::f32 a = 1.f / 3.f;
	//	rnd::f32 b = std::sqrt(8.f / 9.f);
	//	rnd::f32 c = std::sqrt(2.f / 9.f);
	//	rnd::f32 d = std::sqrt(2.f / 3.f);

	//	result.positions.push_back({  0,  0,  1 });
	//	result.positions.push_back({ -c,  d, -a });
	//	result.positions.push_back({ -c, -d, -a });
	//	result.positions.push_back({  b,  0, -a });

	//	result.colors = {
	//		{1.f, 1.f, 1.f, 1.f},
	//		{0.f, 1.f, 1.f, 1.f},
	//		{1.f, 0.f, 1.f, 1.f},
	//		{0.f, 0.f, 1.f, 1.f},
	//	};

	//	result.indices = {
	//		0, 1, 2,
	//		0, 2, 3,
	//		0, 3, 1,
	//		3, 2, 1
	//	};

	//	return result;
	//}

	struct Vertex {
		math::vec3 position;
		math::vec2 texCoord;
		math::vec3 normal;
	};

	static inline mesh load_mesh_from_obj(std::string_view file_path)
	{
		std::vector<Vertex> vertices;
		std::vector<uint16_t> indices;

		std::ifstream file(file_path.data());
		ASSERT(file.is_open(), "Failed to open file {}", file_path);

		std::vector<math::vec3> positions;
		std::vector<math::vec2> texCoords;
		std::vector<math::vec3> normals;
		std::unordered_map<std::string, unsigned int> uniqueVertices;

		std::string line;
		while (std::getline(file, line)) 
		{
			std::istringstream ss(line);
			std::string type;
			ss >> type;

			if (type == "v") { // Vertex position
				math::vec3 pos;
				ss >> pos.x >> pos.y >> pos.z;
				positions.push_back(pos);
			}
			else if (type == "vt") { // Texture coordinate
				math::vec2 tex;
				ss >> tex.x >> tex.y;
				texCoords.push_back(tex);
			}
			else if (type == "vn") { // Normal
				math::vec3 normal;
				ss >> normal.x >> normal.y >> normal.z;
				normals.push_back(normal);
			}
			else if (type == "f") { // Face
				// Store all vertex strings in a temporary vector
				std::vector<std::string> faceVertices;
				std::string vertexStr;
				while (ss >> vertexStr) {
					faceVertices.push_back(vertexStr);
				}

				// Check that the face has at least 3 vertices
				if (faceVertices.size() < 3) {
					std::cerr << "Face with fewer than 3 vertices found." << std::endl;
					continue;
				}

				// Fan triangulation: use the first vertex as a pivot
				for (size_t i = 1; i < faceVertices.size() - 1; ++i) {
					// The triangle consists of vertices: 0, i, i+1
					std::string triIndices[3] = { faceVertices[0], faceVertices[i], faceVertices[i + 1] };

					for (int j = 0; j < 3; j++) {
						const std::string& key = triIndices[j];
						if (uniqueVertices.count(key) == 0) {
							std::istringstream vs(key);
							std::string vIndex, vtIndex, vnIndex;

							std::getline(vs, vIndex, '/');
							std::getline(vs, vtIndex, '/');
							std::getline(vs, vnIndex, '/');

							Vertex vertex{};
							vertex.position = positions[std::stoi(vIndex) - 1];
							vertex.texCoord = texCoords.empty() ? math::vec2(0.0f) : texCoords[std::stoi(vtIndex) - 1];
							vertex.normal = normals.empty() ? math::vec3(0.0f) : normals[std::stoi(vnIndex) - 1];

							unsigned int index = static_cast<unsigned int>(vertices.size());
							uniqueVertices[key] = index;
							vertices.push_back(vertex);
						}
						indices.push_back(uniqueVertices[key]);
					}
				}
			}
		}
		positions.clear();
		texCoords.clear();
		normals.clear();

		// convert from 'Vertex' to Mesh
		for (const Vertex& v : vertices)
		{
			positions.push_back(v.position);
			texCoords.push_back(v.texCoord);
			normals.push_back(v.normal);
		}

		return mesh
		{
			.positions = positions,
			.tex_coords = texCoords,
			.normals = normals,
			.colors = std::vector<math::vec4>(positions.size()),
			.indices = indices,
		};
	}
}