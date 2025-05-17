#pragma once

#include <Engine/engine.hpp>

struct directional_light
{
	math::vec3 direction;
	math::vec3 color;
};

struct point_light 
{
	math::vec3 position;
	math::vec3 color;
	rnd::f32 att_quad;
	rnd::f32 att_lin;
	rnd::f32 att_const;
};