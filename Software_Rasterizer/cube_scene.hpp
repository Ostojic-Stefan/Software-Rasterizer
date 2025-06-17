#pragma once

#include "scene.hpp"
#include "orbit_camera.hpp"
#include "event.hpp"
#include "viewport.hpp"
#include "generic_renderer.hpp"

struct BasicShaderProgram
{
	struct VertexShader
	{
		VSOutput operator()(const VsInput& in) const
		{
			math::vec3 pos = in.Get<math::vec3>(0);
			math::vec3 color = in.Get<math::vec3>(1);

			VSOutput out;
			out.Position = math::mat4::rotation_y(total_time) * math::vec4{ pos.x, pos.y, pos.z, 1.0f };
			out.setVarying<math::vec3>(0, color);

			return out;
		}

	public:
		rnd::f32 total_time = 0.f;
	};

	struct FragmentShader
	{
		math::vec4 operator()(const VSOutput& vsout) const
		{
			math::vec3 color = vsout.getVarying<math::vec3>(0);

			return math::vec4{ color.x, color.y, color.z, 1.0f };
		}
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

	//rnd::orbit_camera _camera;
	//rnd::orbit_camera_controller _cam_ctrl;

	//math::mat4 _projection = math::mat4::perspective(0.1f, 100.f, math::pi32 / 2.f, 800.f / 600.f);

	// gfx::mesh _cube_mesh;
};
