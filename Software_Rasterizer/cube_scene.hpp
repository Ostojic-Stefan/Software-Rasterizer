#pragma once

#include "scene.hpp"
#include "orbit_camera.hpp"
#include "event.hpp"
#include "viewport.hpp"
#include "generic_renderer.hpp"
#include <graphics/texture.hpp>

struct BasicShaderProgram
{
	struct VertexShader
	{
		VSOutput operator()(const VsInput& in) const;
	public:
		math::mat4 _projection = math::mat4::perspective(0.1f, 100.f, math::pi32/2.f, 800.f / 600.f);
		rnd::f32 total_time = 0.f;
	};

	struct FragmentShader
	{
		FragmentShader();
		math::vec4 operator()(const VSOutput& vsout) const;

	public:
		gfx::surface surf;
	};

	VertexShader vs;
	FragmentShader fs;
};

struct cube_plain_scene : iscene
{
	cube_plain_scene(rnd::framebuffer& fb);

	void update(rnd::f32 dt) override;
	void render() override;

private:
	rnd::framebuffer& _fb;
	BasicShaderProgram _shader_program;
	Renderer<BasicShaderProgram> _generic_renderer;
	
	int vboId;
	int iboId;

	//rnd::orbit_camera _camera;
	//rnd::orbit_camera_controller _cam_ctrl;

	//math::mat4 _projection = math::mat4::perspective(0.1f, 100.f, math::pi32 / 2.f, 800.f / 600.f);

	// gfx::mesh _cube_mesh;
};
