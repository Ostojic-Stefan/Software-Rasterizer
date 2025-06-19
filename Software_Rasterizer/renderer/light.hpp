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
	math::vec3 ambient;
	math::vec3 diffuse;
	rnd::f32 diffuse_intensity;

	rnd::f32 att_quad;
	rnd::f32 att_linear;
	rnd::f32 att_const;
};

//cbuffer PointLightCBuf : register(b0)
//{
//	float3 viewLightPos;
//	float3 ambient;
//	float3 diffuseColor;
//	float diffuseIntensity;
//	float attConst;
//	float attLin;
//	float attQuad;
//};