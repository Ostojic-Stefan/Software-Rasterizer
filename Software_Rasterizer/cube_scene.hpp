#pragma once

#include "scene.hpp"
#include "orbit_camera.hpp"
#include "event.hpp"
#include "viewport.hpp"
#include "generic_renderer.hpp"

struct cube_plain_scene : iscene
{
	cube_plain_scene(rnd::framebuffer& fb);

	void update(rnd::f32 dt) override;
	void render() override;

private:
	rnd::framebuffer& _fb;
	Renderer _generic_renderer;
	
	//rnd::orbit_camera _camera;
	//rnd::orbit_camera_controller _cam_ctrl;

	//math::mat4 _projection = math::mat4::perspective(0.1f, 100.f, math::pi32 / 2.f, 800.f / 600.f);

	// gfx::mesh _cube_mesh;
};
