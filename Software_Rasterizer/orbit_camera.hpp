#pragma once

#include <Engine/engine.hpp>

class orbit_camera
{
public:
	friend class orbit_camera_controller;

	orbit_camera(rnd::f32 dist_to_origin)
		: dist_to_origin{ dist_to_origin } {}

	math::vec3 get_position() const
	{
		return math::vec3{
			dist_to_origin * std::sin(phi) * std::cos(theta),
			dist_to_origin * std::cos(phi),
			dist_to_origin * std::sin(phi) * std::sin(theta)
		};
	}

	void zoom(rnd::f32 amount)
	{
		dist_to_origin += amount;
	}

	void rotate_azimuth(rnd::f32 angle_radius)
	{
		theta += angle_radius;
		theta = std::fmodf(theta, 2 * math::pi32);
		if (theta < 0)
			theta += 2 * math::pi32;
	}

	void rotate_polar(rnd::f32 angle_radius)
	{
		phi += angle_radius;

		if (phi < 0.01f) 
			phi = 0.01f;

		if (phi > math::pi32 - 0.01f) 
			phi = math::pi32 - 0.01f;
	}

	math::mat4 get_view_matrix(const math::vec3& look_point = { 0, 0, 0 }) const
	{
		auto pos = get_position();
		auto res = math::mat4::look_at(pos, look_point, math::vec3{ 0, 1, 0 });
		return res;

	}

private:
	rnd::f32 dist_to_origin = 5.0f;
	rnd::f32 phi = math::pi32 / 2.f;
	rnd::f32 theta = math::pi32 / 2.f;
};

class orbit_camera_controller
{
public:
	orbit_camera_controller(orbit_camera& cam)
		:
		camera(cam)
	{
	}

	//void key_pressed_handler(const rnd::key_event& ev)
	//{
	//	switch (ev.code)
	//	{
	//		case rnd::key_code::W: camera.zoom(-0.1f);			 break;
	//		case rnd::key_code::S: camera.zoom(0.1f);			 break;
	//		case rnd::key_code::A: camera.rotate_azimuth(-0.1f); break;
	//		case rnd::key_code::D: camera.rotate_azimuth(0.1f);	 break;
	//	}
	//}

	//void mouse_button_handler(const rnd::mouse_button_event& ev)
	//{
	//	if (ev.down)
	//	{
	//		if (ev.btn == rnd::mouse_btn::left)
	//		{
	//			mouse_held = true;
	//			rnd::i32 x, y;
	//			SDL_GetMouseState(&x, &y);
	//			last_mouse_pos = {(rnd::f32)x, (rnd::f32)y};
	//		}
	//	}
	//	else
	//	{
	//		if (ev.btn == rnd::mouse_btn::left)
	//		{
	//			mouse_held = false;
	//		}
	//	}
	//}

	//void mouse_move_handler(const rnd::mouse_move_event& ev)
	//{
	//	if (mouse_held)
	//	{
	//		rnd::f32 dx = (ev.pos.x - last_mouse_pos.x) * sensitivity;
	//		rnd::f32 dy = (ev.pos.y - last_mouse_pos.y) * sensitivity;

	//		camera.rotate_azimuth(-dx);
	//		camera.rotate_polar(dy);

	//		last_mouse_pos = ev.pos;
	//	}
	//}

private:
	orbit_camera& camera;

	rnd::f32	sensitivity = 0.005f;
	math::vec2	last_mouse_pos = { 0.f, 0.f };
	rnd::b8		mouse_held = false;
};