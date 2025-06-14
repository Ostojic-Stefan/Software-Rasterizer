#pragma once

#include "scene.hpp"
#include "renderer.hpp"
#include "orbit_camera.hpp"
#include "event.hpp"
#include "viewport.hpp"

struct cube_plain_scene : iscene
{
	cube_plain_scene(rnd::framebuffer& fb);

	void update(rnd::f32 dt) override;
	void render() override;

private:
	struct vertex_shader
	{
		inline void bind_model_matrix(const math::mat4& mat) { _model = mat; }
		inline void bind_view_matrix(const math::mat4& mat) { _view = mat; }
		inline void bind_projection_matrix(const math::mat4& mat) { _projection = mat; }

		vertex_output operator()(const vertex_input& in)
		{
			vertex_output ret {};
			ret.world_position = _model * math::vec4(in.position, 1.0f);
			ret.position = _projection * _view * _model * math::vec4(in.position, 1.0f);
			ret.normal = _model * math::vec4(in.normal, 0.0);

			ret.tex_coord = in.tex_coord;
			ret.color = in.color;

			return ret;
		}

	private:
		math::mat4 _model      = math::mat4::identity();
		math::mat4 _view	   = math::mat4::identity();
		math::mat4 _projection = math::mat4::identity();
	};

	struct fragment_shader
	{
		rnd::color operator()(const vertex_output& vs_out)
		{
			return rnd::red;
		}
	};

private:
	rnd::framebuffer& _fb;
	renderer<vertex_shader, fragment_shader> _renderer;
	viewport _viewport = { 0, 0, 800, 600 };

	orbit_camera _camera;
	orbit_camera_controller _cam_ctrl;

	math::mat4 _projection = math::mat4::perspective(0.1f, 100.f, math::pi32 / 2.f, 800.f / 600.f);

	vertex_shader _vs;
	fragment_shader _fs;

	gfx::mesh _cube_mesh;
};
