#include "cube.hpp"
#include <vector>

namespace rasterizer
{

	static vector3f cube_positions[] =
	{
		// -X face
		{-1.f, -1.f, -1.f},
		{-1.f,  1.f, -1.f},
		{-1.f, -1.f,  1.f},
		{-1.f,  1.f,  1.f},

		// +X face
		{ 1.f, -1.f, -1.f},
		{ 1.f,  1.f, -1.f},
		{ 1.f, -1.f,  1.f},
		{ 1.f,  1.f,  1.f},

		// -Y face
		{-1.f, -1.f, -1.f},
		{ 1.f, -1.f, -1.f},
		{-1.f, -1.f,  1.f},
		{ 1.f, -1.f,  1.f},

		// +Y face
		{-1.f,  1.f, -1.f},
		{ 1.f,  1.f, -1.f},
		{-1.f,  1.f,  1.f},
		{ 1.f,  1.f,  1.f},

		// -Z face
		{-1.f, -1.f, -1.f},
		{ 1.f, -1.f, -1.f},
		{-1.f,  1.f, -1.f},
		{ 1.f,  1.f, -1.f},

		// +Z face
		{-1.f, -1.f,  1.f},
		{ 1.f, -1.f,  1.f},
		{-1.f,  1.f,  1.f},
		{ 1.f,  1.f,  1.f},
	};

	static vector3f cube_normals[] =
	{
		// -X face
		{-1.f,  0.f,  0.f},
		{-1.f,  0.f,  0.f},
		{-1.f,  0.f,  0.f},
		{-1.f,  0.f,  0.f},

		// +X face
		{ 1.f,  0.f,  0.f},
		{ 1.f,  0.f,  0.f},
		{ 1.f,  0.f,  0.f},
		{ 1.f,  0.f,  0.f},

		// -Y face
		{ 0.f, -1.f,  0.f},
		{ 0.f, -1.f,  0.f},
		{ 0.f, -1.f,  0.f},
		{ 0.f, -1.f,  0.f},

		// +Y face
		{ 0.f,  1.f,  0.f},
		{ 0.f,  1.f,  0.f},
		{ 0.f,  1.f,  0.f},
		{ 0.f,  1.f,  0.f},

		// -Z face
		{ 0.f,  0.f, -1.f},
		{ 0.f,  0.f, -1.f},
		{ 0.f,  0.f, -1.f},
		{ 0.f,  0.f, -1.f},

		// +Z face
		{ 0.f,  0.f,  1.f},
		{ 0.f,  0.f,  1.f},
		{ 0.f,  0.f,  1.f},
		{ 0.f,  0.f,  1.f},
	};

	static vector4f cube_colors[] =
	{
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

	static vector2f cube_texcoords[] =
	{
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

	static std::uint32_t cube_indices[] =
	{
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

	const mesh cube
	{
		.positions = {cube_positions},
		.normals = {cube_normals},
		.colors = {cube_colors},
		.texcoords = {cube_texcoords},
		.indices = cube_indices,
		.count = 36,
	};




	static const vector3f positions[] = {
		{ -0.5f, -0.5f, 0.0f },	// bottom-left
		{  0.5f, -0.5f, 0.0f },	// bottom-right
		{ -0.5f,  0.5f, 0.0f },	// top-left 
		{  0.5f,  0.5f, 0.0f },	// top-right
	};

	static const vector4f colors[] = {
		{ 0.f, 0.f, 0.f, 1.f },
		{ 0.f, 1.f, 0.f, 1.f },
		{ 1.f, 0.f, 0.f, 1.f },
		{ 1.f, 1.f, 0.f, 1.f },
	};

	static const vector2f tex_coords[] = {
		{ 0, 0 },
		{ 1, 0 },
		{ 0, 1 },
		{ 1, 1 },
	};

	static const vector3f normals[] = {
		{ 0, 0, 1 },
		{ 0, 0, 1 },
		{ 0, 0, 1 },
		{ 0, 0, 1 },
	};

	static const uint32_t indices[] = {
		0,1,2,
		1,3,2
	};

	const mesh plane{ { positions }, { normals }, {colors}, { tex_coords }, indices, 6 };

}
