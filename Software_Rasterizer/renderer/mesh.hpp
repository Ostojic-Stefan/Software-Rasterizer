#pragma once

#include "math/vector.hpp"
#include "types.hpp"
#include <vector>
#include "handle_manager.hpp"
#include "generic_renderer.hpp"

namespace gfx
{
	struct vertex
	{
		math::vec3 position;
		math::vec3 normal;
		math::vec2 tex_coords;

		math::vec3 tangent;
		math::vec3 bitangent;
	};

	struct mesh
	{
		mesh(std::vector<vertex> vertices, std::vector<rnd::u16> indices)
			:
			vertices(std::move(vertices)),
			indices(std::move(indices))
		{}

		std::vector<vertex> vertices;
		std::vector<rnd::u16> indices;

		rnd::resource_handle vboid;
		rnd::resource_handle iboid;
	};
}