#pragma once

#include "scene.hpp"
#include "orbit_camera.hpp"
#include "event.hpp"
#include "renderer/viewport.hpp"
#include "renderer/generic_renderer.hpp"
#include <graphics/texture.hpp>

struct BasicShaderProgram
{
	struct VertexShader
	{
		VSOutput operator()(const VSInput& in) const;

		void bindViewMatrix(const math::mat4& view);
	public:
		math::mat4 _view;
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
	
	rnd::resource_handle vboId;
	rnd::resource_handle iboId;

	rnd::orbit_camera _camera;
	rnd::orbit_camera_controller _cam_ctrl;
};
