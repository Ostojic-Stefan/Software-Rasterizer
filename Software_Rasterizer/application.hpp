#pragma once

#include <Engine/engine.hpp>

#include "viewport.hpp"
#include "renderer.hpp"
#include "orbit_camera.hpp"
#include "movement_camera.hpp"
#include "another_renderer.hpp"

class application
{
public:
	application();
	~application();
	void run();
	void update(rnd::f32 dt);
	void render();
private:
	void render_mesh(
		const gfx::mesh& mesh,
		vertex_shader& vs,
		fragment_shader& fs,
		const math::mat4& tform
	);
	void render_point_light(const math::mat4& model);
private:
	rnd::sdl_window window;
	rnd::framebuffer fb;
	rnd::event_handler event_handler;

	math::mat4 proj = math::mat4::perspective(0.1f, 100.f, math::pi32/2.f, 800.f / 600.f);
	// math::mat4 proj = math::mat4::orthographic(-10.0f, 10.0f, -10.0f, 10.0f, 1.f, 100.0f);


	rnd::b8 running = true;
	rnd::f32 dt = 0.f;
	rnd::f32 total_time = 0.f;

	struct viewport viewport = { 0, 0, 800, 600 };

	//vertex_shader	vs;
	//fragment_shader fs;

	renderer<vertex_shader, fragment_shader> object_renderer;
	renderer<vertex_shader_point_light, fragment_shader_point_light> light_renderer;

	orbit_camera camera;
	orbit_camera_controller cam_ctrl;

	movement_camera move_cam;
	rnd::f32 lastX = 0.f;
	rnd::f32 lastY = 0.f;
	rnd::b8 firstMouse = false;
	
	gfx::texture diffuse_tex;
	gfx::texture specular_tex;

	math::vec3 light_pos1 = { 0.0f, 0.0f, -1.0f };
	math::vec3 light_pos2 = { 0.0f, 0.0f, -1.0f };

	// gfx::mesh sphere_mesh = gfx::get_sphere_mesh(10, 10);
	gfx::mesh sphere_mesh = gfx::load_mesh_from_obj("../assets/sphere.obj");
	// 
	//gfx::mesh cube_mesh = gfx::get_cube_mesh(1.f);
	gfx::mesh cube_mesh = gfx::load_mesh_from_obj("../assets/cube.obj");
	// 
	//gfx::mesh mesh = gfx::get_tetrahedron_mesh();
	//gfx::mesh triangle_mesh = gfx::get_triangle_mesh();
	//gfx::mesh plane_mesh = gfx::get_plane_mesh();
	gfx::mesh plane_mesh = gfx::load_mesh_from_obj("../assets/plane.obj");
	gfx::mesh mesh = gfx::load_mesh_from_obj("../assets/drone.obj");

	gfx::mesh triangle_mesh = gfx::get_triangle_mesh();

	rnd::i32 mip_level = 0;
};
