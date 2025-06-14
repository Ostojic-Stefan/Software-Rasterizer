#include "pch.h"

#include "input.hpp"

namespace rnd::input
{
	struct internal_state_keyboard
	{
		rnd::b8 keys_down[256];
		rnd::b8 keys_down_prev[256];
	};

	struct internal_state_mouse
	{
		math::vec2 mouse_pos;
		rnd::b8 mouse_btn_down[3];
		rnd::b8 mouse_btn_down_prev[3];
		rnd::f32 mouse_wheel_delta = 0.f;
	};

	static internal_state_keyboard key_state;
	static internal_state_mouse mouse_state;

	void init()
	{
		memset(key_state.keys_down, 0, 256);
		memset(key_state.keys_down_prev, 0, 256);

		memset(mouse_state.mouse_btn_down, 0, 3);
		memset(mouse_state.mouse_btn_down_prev, 0, 3);
	}

	void shutdown()
	{
	}

	void update()
	{
		memcpy(key_state.keys_down_prev, key_state.keys_down, 256);
		memcpy(mouse_state.mouse_btn_down_prev, mouse_state.mouse_btn_down, 3);

		mouse_state.mouse_wheel_delta = 0.f;
	}

	rnd::b8 is_key_pressed(key_code key)
	{
		return key_state.keys_down[(int)key] && !key_state.keys_down_prev[(int)key];
	}

	rnd::b8 is_key_down(key_code key)
	{
		return key_state.keys_down[(int)key];
	}

	rnd::b8 is_key_released(key_code key)
	{
		return key_state.keys_down_prev[(int)key] && !key_state.keys_down[(int)key];
	}

	rnd::b8 is_key_up(key_code key)
	{
		return !key_state.keys_down[(int)key];
	}

	void process_key(key_code code, rnd::b8 is_down)
	{
		key_state.keys_down[(int)code] = is_down;
	}

	math::vec2 get_mouse_pos()
	{
		return mouse_state.mouse_pos;
	}

	rnd::b8 is_mouse_down(mouse_btn btn)
	{
		return mouse_state.mouse_btn_down[(int)btn];
	}

	rnd::b8 is_mouse_pressed(mouse_btn btn)
	{
		return mouse_state.mouse_btn_down[(int)btn] && !mouse_state.mouse_btn_down_prev[(int)btn];
	}

	rnd::b8 is_mouse_up(mouse_btn btn)
	{
		return !mouse_state.mouse_btn_down[(int)btn];
	}

	rnd::b8 is_mouse_released(mouse_btn btn)
	{
		return !mouse_state.mouse_btn_down[(int)btn] && mouse_state.mouse_btn_down_prev[(int)btn];
	}

	void process_mouse_move(math::vec2 pos)
	{
		mouse_state.mouse_pos = pos;
	}

	void process_mouse(mouse_btn btn, rnd::b8 is_down)
	{
		mouse_state.mouse_btn_down[(int)btn] = is_down;
	}

	void process_mouse_wheel(rnd::f32 value)
	{
		mouse_state.mouse_wheel_delta += value;
	}

	rnd::f32 get_mouse_wheel_delta()
	{
		return mouse_state.mouse_wheel_delta;
	}
}

