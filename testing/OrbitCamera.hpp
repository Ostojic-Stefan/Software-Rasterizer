#pragma once

#include <SDL2/SDL.h>

#include "vector.hpp"
#include "matrix.hpp"

using namespace rasterizer;

class orbit_camera
{
public:

	friend class orbit_camera_controller;

	orbit_camera(float dist_to_origin)
		: dist_to_origin{ dist_to_origin } {}

	vector3f get_position() const
	{
		return vector3f{
			dist_to_origin * std::sin(phi) * std::cos(theta),
			dist_to_origin * std::cos(phi),
			dist_to_origin * std::sin(phi) * std::sin(theta)
		};
	}

	void zoom(float amount)
	{
		dist_to_origin += amount;
	}

	void rotate_azimuth(float angle_radius)
	{
		theta += angle_radius;
		theta = std::fmodf(theta, 2 * 3.1415f);
		if (theta < 0)
			theta += 2 * 3.1415f;
	}

	void rotate_polar(float angle_radius)
	{
		phi += angle_radius;

		if (phi < 0.01f)
			phi = 0.01f;

		if (phi > 3.1415f - 0.01f)
			phi = 3.1415f - 0.01f;
	}

	matrix4x4f get_view_matrix(const vector3f& look_point = { 0, 0, 0 }) const
	{
		auto pos = get_position();
		auto res = lookAt(pos, look_point, vector3f{ 0, 1, 0 });
		return res;
	}

private:
	float dist_to_origin = 5.0f;
	float phi = 3.1415f / 2.f;
	float theta = 3.1415f / 2.f;
};

class orbit_camera_controller
{
public:
	orbit_camera_controller(orbit_camera& cam)
		:
		camera(cam)
	{
	}

	void key_pressed_handler(const int keycode)
	{
		switch (keycode)
		{
		case SDLK_w: camera.zoom(-0.1f);			 break;
		case SDLK_s: camera.zoom(0.1f);			 break;
		case SDLK_a: camera.rotate_azimuth(-0.1f); break;
		case SDLK_d: camera.rotate_azimuth(0.1f);	 break;
		}
	}

	void mouse_button_handler(const int mouseButton, bool isDown)
	{
		if (isDown)
		{
			if (mouseButton == SDL_BUTTON_LEFT)
			{
				mouse_held = true;
				int x, y;
				SDL_GetMouseState(&x, &y);
				last_mouse_pos = { (float)x, (float)y };
			}
		}
		else
		{
			if (mouseButton == SDL_BUTTON_LEFT)
			{
				mouse_held = false;
			}
		}
	}

	void mouse_move_handler(const int mousePosX, int mousePosY)
	{
		if (mouse_held)
		{
			float dx = (mousePosX - last_mouse_pos.x) * sensitivity;
			float dy = (mousePosY - last_mouse_pos.y) * sensitivity;

			camera.rotate_azimuth(dx);
			camera.rotate_polar(-dy);

			last_mouse_pos = { (float) mousePosX, (float) mousePosY};
		}
	}

private:
	orbit_camera& camera;

	float	sensitivity = 0.005f;
	vector2f	last_mouse_pos = { 0.f, 0.f };
	bool		mouse_held = false;
};