#pragma once

#include "types.hpp"
#include "math/vector.hpp"
#include "math/matrix.hpp"
#include "math/util.hpp"
#include "input.hpp"

namespace rnd
{
	class orbit_camera
	{
	public:
		friend class orbit_camera_controller;

		orbit_camera(f32 dist_to_origin)
			: dist_to_origin{ dist_to_origin }
			, phi{ math::pi32 / 2.f }
			, theta{ 0.0f }         // start theta at 0
		{}

		math::vec3 get_position() const
		{
			f32 x = dist_to_origin * std::sin(phi) * std::sin(theta);
			f32 y = dist_to_origin * std::cos(phi);
			f32 z = dist_to_origin * std::sin(phi) * std::cos(theta);
			return math::vec3{ x, y, z };
		}

		void zoom(f32 amount)
		{
			dist_to_origin += amount;
		}

		void rotate_azimuth(f32 angle_radius)
		{
			theta += angle_radius;
			theta = std::fmodf(theta, 2 * math::pi32);
			if (theta < 0)
				theta += 2 * math::pi32;
		}

		void rotate_polar(f32 angle_radius)
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
			return math::mat4::look_at(pos, look_point, math::vec3{ 0, 1, 0 });
		}

	private:
		f32 dist_to_origin = 0.0f;
		f32 phi = math::pi32 / 2.f;
		f32 theta = math::pi32 / 2.f;
	};

	class orbit_camera_controller
	{
	public:
		orbit_camera_controller(orbit_camera& cam)
			:
			camera(cam)
		{}

		void update(f32 dt)
		{
			if (input::is_key_down(input::key_code::W)) camera.zoom(-move_speed * dt);
			if (input::is_key_down(input::key_code::S)) camera.zoom(move_speed * dt);
			if (input::is_key_down(input::key_code::A)) camera.rotate_azimuth(-rot_speed * dt);
			if (input::is_key_down(input::key_code::D)) camera.rotate_azimuth(rot_speed * dt);

			if (input::is_mouse_pressed(input::mouse_btn::left))
			{
				mouse_held = true;
				last_mouse_pos = input::get_mouse_pos();
			}
			else if (input::is_mouse_released(input::mouse_btn::left))
			{
				mouse_held = false;
			}

			if (mouse_held)
			{
				math::vec2 mouse_pos = input::get_mouse_pos();
				f32 dx = (mouse_pos.x - last_mouse_pos.x) * sensitivity;
				f32 dy = (mouse_pos.y - last_mouse_pos.y) * sensitivity;

				camera.rotate_azimuth(-dx);
				camera.rotate_polar(-dy);

				last_mouse_pos = mouse_pos;
			}
		}

	private:
		orbit_camera& camera;

		f32	sensitivity = 0.005f;
		f32 rot_speed = 5.f;
		f32 move_speed = 5.f;
		math::vec2	last_mouse_pos = { 0.f, 0.f };
		b8		mouse_held = false;
	};
}


