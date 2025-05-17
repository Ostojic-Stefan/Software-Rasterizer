#pragma once

#include "vector.hpp"
#include "attribute.hpp"

namespace rasterizer
{

	struct mesh
	{
		static constexpr vector3f default_normal {0.f, 0.f, 1.f};
		static constexpr vector4f default_color {1.f, 1.f, 1.f, 1.f};
		static constexpr vector2f default_texcoord {0.5f, 0.5f};

		attribute<vector3f> positions = {};
		attribute<vector3f> normals = {&default_normal, 1};
		attribute<vector4f> colors = {&default_color, 1};
		attribute<vector2f> texcoords = {&default_texcoord, 1};
		std::uint32_t const * indices = nullptr;
		std::uint32_t count = 0;
	};

}
