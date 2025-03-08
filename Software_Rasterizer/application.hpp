#pragma once

#include <Engine/engine.hpp>

#include "viewport.hpp"
#include "renderer.hpp"
#include "orbit_camera.hpp"

struct vertex_shader 
{
	inline void bind_model_matrix(const math::mat4& mat) { model = mat; }
	inline void bind_view_matrix(const math::mat4& mat) { view = mat; }
	inline void bind_projection_matrix(const math::mat4& mat) { projection = mat; }

	vertex_output operator()(const vertex_input& v)
	{
		vertex_output ret;
		
		ret.world_position = model * math::vec4(v.position, 1.0f);
		ret.position = projection * view * model * math::vec4(v.position, 1.0f);

		ret.normal = model * math::vec4(v.normal, 0.0);

		ret.tex_coord = v.tex_coord;
		ret.color = v.color;
		 
		return ret;
	}

private:
	math::mat4 model		= math::mat4::identity();
	math::mat4 view			= math::mat4::identity();
	math::mat4 projection	= math::mat4::identity();
};

struct fragment_shader 
{
	rnd::color operator()(const vertex_output& vert)
	{
		math::vec3 base_color(0.5f);
		math::vec3 lighting = math::vec3(0.0f);
		math::vec3 normal = math::normalize(vert.normal);

		// ambient
		math::vec3 ambient = math::vec3(0.3f);

		// hemi
		math::vec3 sky_color = math::vec3(0.0f, 0.3f, 0.6f);
		math::vec3 ground_color = math::vec3(0.6f, 0.3f, 0.1f);

		rnd::f32 hemi_mix = math::remap(normal.y, -1.0f, 1.0f, 0.0f, 1.0f);
		math::vec3 hemi = math::lerp(ground_color, sky_color, hemi_mix);

		// diffuse lighting
		math::vec3 light_dir = math::normalize(math::vec3(0.f, 0.f, 1.f));
		math::vec3 light_color = math::vec3(1.f, 1.f, 0.9f);
		rnd::f32 dp = std::max(0.f, math::dot(light_dir, normal));
		math::vec3 diffuse = dp * light_color;

		// phong specular
		math::vec3 view_dir = math::normalize(cam_pos - vert.world_position);
		math::vec3 r = math::normalize(math::reflect(-light_dir, normal));
		rnd::f32 phong_value = std::max(0.f, math::dot(view_dir, r));
		phong_value = std::pow(phong_value, 32.f);
		math::vec3 specular = math::vec3(phong_value);

		lighting = ambient + 0.f * hemi + diffuse + specular;
		math::vec3 color = base_color * lighting;

		return rnd::to_color(color);
	}

	void bind_camera_position(const math::vec3& cam_pos)
	{
		this->cam_pos = cam_pos;
	}

	void bind_texture(gfx::texture* tex)
	{
		this->tex = tex;
	}

private:
	gfx::texture* tex = nullptr;
	math::vec3 cam_pos;
};

class application
{
public:
	application();
	~application();
	void run();
	void update(rnd::f32 dt);
	void render();
private:
	rnd::sdl_window window;
	rnd::framebuffer fb;
	rnd::event_handler event_handler;

	rnd::b8 running = true;
	rnd::f32 total_time = 0.f;

	struct viewport viewport = { 0, 0, 800, 600 };

	vertex_shader	vs;
	fragment_shader fs;
	renderer<vertex_shader, fragment_shader> renderer;

	orbit_camera camera;
	orbit_camera_controller cam_ctrl;
	
	gfx::texture tex;

	//gfx::mesh mesh = gfx::get_sphere_mesh(20, 20);
	 //gfx::mesh mesh = gfx::get_cube_mesh(1.0f);
	//gfx::mesh mesh = gfx::get_tetrahedron_mesh();
	 //gfx::mesh mesh = gfx::get_triangle_mesh();
	 //gfx::mesh mesh = gfx::get_plane_mesh();
	gfx::mesh mesh = gfx::load_mesh_from_obj("../assets/crab.obj");
};
