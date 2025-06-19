#pragma once

#include "scene.hpp"
#include "orbit_camera.hpp"
#include "event.hpp"
#include "renderer/viewport.hpp"
#include "renderer/generic_renderer.hpp"
#include <graphics/texture.hpp>
#include "renderer/model.hpp"
#include "renderer/light.hpp"

struct model_shader_program
{
	struct vertex_shader
	{
		VSOutput operator()(const VSInput& in) const;

		void bindViewMatrix(const math::mat4& view);
	public:
		math::mat4 _view = math::mat4::identity();
		math::mat4 _projection = math::mat4::perspective(0.1f, 100.f, math::pi32 / 2.f, 800.f / 600.f);
		rnd::f32 total_time = 0.f;
	};

	struct fragment_shader
	{
		fragment_shader();
		void bind_point_light(const point_light& p_light);
		void bind_view_direction(const math::vec3& view_dir);

		math::vec4 operator()(const VSOutput& vsout) const;

	public:
		gfx::surface surf;
		math::vec3 cam_pos;
		point_light p_light;
	};

	vertex_shader vs;
	fragment_shader fs;
};

struct mode_scene : iscene
{
	mode_scene(rnd::framebuffer& fb);

	void update(rnd::f32 dt) override;
	void render() override;

private:
	rnd::framebuffer& _fb;
	model_shader_program _shader_program;
	Renderer<model_shader_program> _generic_renderer;

	rnd::resource_handle vboId;
	rnd::resource_handle iboId;
	
	rnd::orbit_camera _camera;
	rnd::orbit_camera_controller _cam_ctrl;

	gfx::model the_model;
	point_light _point_light;
};