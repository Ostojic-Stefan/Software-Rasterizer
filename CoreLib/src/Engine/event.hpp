#pragma once

#include "math/point.hpp"
#include "math/vector.hpp"

namespace rnd
{
	struct mouse_move_event
	{
		math::vec2i pos;
		math::vec2i rel;
	};

	enum class mouse_btn { left, middle, right };
	struct mouse_button_event
	{
		mouse_btn btn;
		bool down;
	};

	enum class key_code
	{
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,
		NUM_1,
		NUM_2,
		NUM_3,
		NUM_4,
		NUM_5,
		NUM_6,
		NUM_7,
		NUM_8,
		NUM_9,
		NUM_0,
		RETURN,
		ESCAPE,
		BACKSPACE,
		TAB,
		SPACE,
		MINUS,
		EQUALS,
		LEFTBRACKET,
		RIGHTBRACKET,
		BACKSLASH,
		NONUSHASH,
		SEMICOLON,
		APOSTROPHE,
		BACKQUOTE,
		COMMA,
		PERIOD,
		SLASH,
		CAPSLOCK,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		PRINTSCREEN,
		SCROLLLOCK,
		PAUSE,
		INSERT,
		HOME,
		PAGEUP,
		DELETE,
		END,
		PAGEDOWN,
		RIGHT,
		LEFT,
		DOWN,
		UP,
		NUMLOCKCLEAR,
		KP_DIVIDE,
		KP_MULTIPLY,
		KP_MINUS,
		KP_PLUS,
		KP_ENTER,
		KP_1,
		KP_2,
		KP_3,
		KP_4,
		KP_5,
		KP_6,
		KP_7,
		KP_8,
		KP_9,
		KP_0,
		KP_PERIOD,
		APPLICATION,
		MUTE,
		VOLUMEUP,
		VOLUMEDOWN,
		LCTRL,
		LSHIFT,
		LALT,
		LGUI,
		RCTRL,
		RSHIFT,
		RALT,
		RGUI,
	};

	struct key_event
	{
		key_code code;
		bool down;
	};

	struct window_resize_event
	{
		rnd::u32 width;
		rnd::u32 height;
	};

	struct event_handler
	{
		friend static inline rnd::b8 poll_events(event_handler& handler);
			
		inline void bind_mouse_move_cb(std::function<void(const rnd::mouse_move_event&)> callback)
		{
			mouse_move_cb = callback;
		}
		inline void bind_key_press_cb(std::function<void(const rnd::key_event&)> callback)
		{
			key_press_cb = callback;
		}
		inline void bind_mouse_pressed_cb(std::function<void(const rnd::mouse_button_event&)> callback)
		{
			mouse_pressed_cb = callback;
		}
		inline void bind_window_resized_cb(std::function<void(const rnd::window_resize_event&)> callback)
		{
			window_resized_cb = callback;
		}
	private:
		std::function<void(const rnd::mouse_move_event&)>		mouse_move_cb;
		std::function<void(const rnd::key_event&)>				key_press_cb;
		std::function<void(const rnd::mouse_button_event&)>		mouse_pressed_cb;
		std::function<void(const rnd::window_resize_event&)>	window_resized_cb;
	};

	static inline std::optional<mouse_btn> convert_mousecode(Uint8 btn)
	{
		switch (btn)
		{
		case SDL_BUTTON_LEFT:
			return mouse_btn::left;
		case SDL_BUTTON_MIDDLE:
			return mouse_btn::middle;
		case SDL_BUTTON_RIGHT:
			return mouse_btn::right;
		default:
			return std::nullopt;
		}
	}

	static inline std::optional<key_code> convert_keycode(SDL_Keysym key)
	{
		switch (key.scancode)
		{
		case SDL_SCANCODE_A:            return key_code::A;
		case SDL_SCANCODE_B:            return key_code::B;
		case SDL_SCANCODE_C:            return key_code::C;
		case SDL_SCANCODE_D:            return key_code::D;
		case SDL_SCANCODE_E:            return key_code::E;
		case SDL_SCANCODE_F:            return key_code::F;
		case SDL_SCANCODE_G:            return key_code::G;
		case SDL_SCANCODE_H:            return key_code::H;
		case SDL_SCANCODE_I:            return key_code::I;
		case SDL_SCANCODE_J:            return key_code::J;
		case SDL_SCANCODE_K:            return key_code::K;
		case SDL_SCANCODE_L:            return key_code::L;
		case SDL_SCANCODE_M:            return key_code::M;
		case SDL_SCANCODE_N:            return key_code::N;
		case SDL_SCANCODE_O:            return key_code::O;
		case SDL_SCANCODE_P:            return key_code::P;
		case SDL_SCANCODE_Q:            return key_code::Q;
		case SDL_SCANCODE_R:            return key_code::R;
		case SDL_SCANCODE_S:            return key_code::S;
		case SDL_SCANCODE_T:            return key_code::T;
		case SDL_SCANCODE_U:            return key_code::U;
		case SDL_SCANCODE_V:            return key_code::V;
		case SDL_SCANCODE_W:            return key_code::W;
		case SDL_SCANCODE_X:            return key_code::X;
		case SDL_SCANCODE_Y:            return key_code::Y;
		case SDL_SCANCODE_Z:            return key_code::Z;
		case SDL_SCANCODE_1:            return key_code::NUM_1;
		case SDL_SCANCODE_2:            return key_code::NUM_2;
		case SDL_SCANCODE_3:            return key_code::NUM_3;
		case SDL_SCANCODE_4:            return key_code::NUM_4;
		case SDL_SCANCODE_5:            return key_code::NUM_5;
		case SDL_SCANCODE_6:            return key_code::NUM_6;
		case SDL_SCANCODE_7:            return key_code::NUM_7;
		case SDL_SCANCODE_8:            return key_code::NUM_8;
		case SDL_SCANCODE_9:            return key_code::NUM_9;
		case SDL_SCANCODE_0:            return key_code::NUM_0;
		case SDL_SCANCODE_RETURN:       return key_code::RETURN;
		case SDL_SCANCODE_ESCAPE:       return key_code::ESCAPE;
		case SDL_SCANCODE_BACKSPACE:    return key_code::BACKSPACE;
		case SDL_SCANCODE_TAB:          return key_code::TAB;
		case SDL_SCANCODE_SPACE:        return key_code::SPACE;
		case SDL_SCANCODE_MINUS:        return key_code::MINUS;
		case SDL_SCANCODE_EQUALS:       return key_code::EQUALS;
		case SDL_SCANCODE_LEFTBRACKET:  return key_code::LEFTBRACKET;
		case SDL_SCANCODE_RIGHTBRACKET: return key_code::RIGHTBRACKET;
		case SDL_SCANCODE_BACKSLASH:    return key_code::BACKSLASH;
		case SDL_SCANCODE_NONUSHASH:    return key_code::NONUSHASH;
		case SDL_SCANCODE_SEMICOLON:    return key_code::SEMICOLON;
		case SDL_SCANCODE_APOSTROPHE:   return key_code::APOSTROPHE;
		case SDL_SCANCODE_GRAVE:        return key_code::BACKQUOTE;
		case SDL_SCANCODE_COMMA:        return key_code::COMMA;
		case SDL_SCANCODE_PERIOD:       return key_code::PERIOD;
		case SDL_SCANCODE_SLASH:        return key_code::SLASH;
		case SDL_SCANCODE_CAPSLOCK:     return key_code::CAPSLOCK;
		case SDL_SCANCODE_F1:           return key_code::F1;
		case SDL_SCANCODE_F2:           return key_code::F2;
		case SDL_SCANCODE_F3:           return key_code::F3;
		case SDL_SCANCODE_F4:           return key_code::F4;
		case SDL_SCANCODE_F5:           return key_code::F5;
		case SDL_SCANCODE_F6:           return key_code::F6;
		case SDL_SCANCODE_F7:           return key_code::F7;
		case SDL_SCANCODE_F8:           return key_code::F8;
		case SDL_SCANCODE_F9:           return key_code::F9;
		case SDL_SCANCODE_F10:          return key_code::F10;
		case SDL_SCANCODE_F11:          return key_code::F11;
		case SDL_SCANCODE_F12:          return key_code::F12;
		case SDL_SCANCODE_PRINTSCREEN:  return key_code::PRINTSCREEN;
		case SDL_SCANCODE_SCROLLLOCK:   return key_code::SCROLLLOCK;
		case SDL_SCANCODE_PAUSE:        return key_code::PAUSE;
		case SDL_SCANCODE_INSERT:       return key_code::INSERT;
		case SDL_SCANCODE_HOME:         return key_code::HOME;
		case SDL_SCANCODE_PAGEUP:       return key_code::PAGEUP;
		case SDL_SCANCODE_DELETE:       return key_code::DELETE;
		case SDL_SCANCODE_END:          return key_code::END;
		case SDL_SCANCODE_PAGEDOWN:     return key_code::PAGEDOWN;
		case SDL_SCANCODE_RIGHT:        return key_code::RIGHT;
		case SDL_SCANCODE_LEFT:         return key_code::LEFT;
		case SDL_SCANCODE_DOWN:         return key_code::DOWN;
		case SDL_SCANCODE_UP:           return key_code::UP;
		case SDL_SCANCODE_NUMLOCKCLEAR: return key_code::NUMLOCKCLEAR;
		case SDL_SCANCODE_KP_DIVIDE:    return key_code::KP_DIVIDE;
		case SDL_SCANCODE_KP_MULTIPLY:  return key_code::KP_MULTIPLY;
		case SDL_SCANCODE_KP_MINUS:     return key_code::KP_MINUS;
		case SDL_SCANCODE_KP_PLUS:      return key_code::KP_PLUS;
		case SDL_SCANCODE_KP_ENTER:     return key_code::KP_ENTER;
		case SDL_SCANCODE_KP_1:         return key_code::KP_1;
		case SDL_SCANCODE_KP_2:         return key_code::KP_2;
		case SDL_SCANCODE_KP_3:         return key_code::KP_3;
		case SDL_SCANCODE_KP_4:         return key_code::KP_4;
		case SDL_SCANCODE_KP_5:         return key_code::KP_5;
		case SDL_SCANCODE_KP_6:         return key_code::KP_6;
		case SDL_SCANCODE_KP_7:         return key_code::KP_7;
		case SDL_SCANCODE_KP_8:         return key_code::KP_8;
		case SDL_SCANCODE_KP_9:         return key_code::KP_9;
		case SDL_SCANCODE_KP_0:         return key_code::KP_0;
		case SDL_SCANCODE_KP_PERIOD:    return key_code::KP_PERIOD;
		case SDL_SCANCODE_APPLICATION:  return key_code::APPLICATION;
		case SDL_SCANCODE_MUTE:         return key_code::MUTE;
		case SDL_SCANCODE_VOLUMEUP:     return key_code::VOLUMEUP;
		case SDL_SCANCODE_VOLUMEDOWN:   return key_code::VOLUMEDOWN;
		case SDL_SCANCODE_LCTRL:        return key_code::LCTRL;
		case SDL_SCANCODE_LSHIFT:       return key_code::LSHIFT;
		case SDL_SCANCODE_LALT:         return key_code::LALT;
		case SDL_SCANCODE_LGUI:         return key_code::LGUI;
		case SDL_SCANCODE_RCTRL:        return key_code::RCTRL;
		case SDL_SCANCODE_RSHIFT:       return key_code::RSHIFT;
		case SDL_SCANCODE_RALT:         return key_code::RALT;
		case SDL_SCANCODE_RGUI:         return key_code::RGUI;
		default:
			return std::nullopt;
		}
	}

	static inline rnd::b8 poll_events(event_handler& handler)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT: return false;
			case SDL_MOUSEMOTION: 
				auto data = mouse_move_event{
					.pos = { event.motion.x, event.motion.y },
					.rel = { event.motion.xrel, event.motion.yrel }
				};
				if (handler.mouse_move_cb) 
					handler.mouse_move_cb(data);
				return true;
			case SDL_MOUSEBUTTONDOWN:
				if (auto btn = convert_mousecode(event.button.button))
				{
					auto data = mouse_button_event{
							.btn = *btn,
							.down = true
					};
					if (handler.mouse_pressed_cb) 
						handler.mouse_pressed_cb(data);
				}
				return true;
			case SDL_MOUSEBUTTONUP:
				if (auto btn = convert_mousecode(event.button.button))
				{
					auto data = mouse_button_event{
						.btn = *btn,
						.down = false
					};
					if (handler.mouse_pressed_cb) 
						handler.mouse_pressed_cb(data);
				}
				return true;
			case SDL_KEYDOWN:
				if (auto key = convert_keycode(event.key.keysym))
				{
					auto data = key_event{
						.code = *key,
						.down = true
					};
					if (handler.key_press_cb) 
						handler.key_press_cb(data);
				}
				return true;
			case SDL_KEYUP:
				if (auto key = convert_keycode(event.key.keysym))
				{
					auto data = key_event{
						.code = *key,
						.down = false
					};
					if (handler.key_press_cb) 
						handler.key_press_cb(data);
				}
				return true;
			case SDL_WINDOWEVENT: 
				switch (event.window.event)
				{
				case SDL_WINDOWEVENT_CLOSE: return false;
				case SDL_WINDOWEVENT_RESIZED:
					LOG("Window Resized to: {}, {}", event.window.data1, event.window.data2);
					if (handler.window_resized_cb) 
						handler.window_resized_cb(window_resize_event{ (u32)event.window.data1, (u32)event.window.data2 });
					return true;
				case SDL_WINDOWEVENT_SIZE_CHANGED:
					LOG("Window Resized to: {}, {}", event.window.data1, event.window.data2);
					if (handler.window_resized_cb)
						handler.window_resized_cb(window_resize_event{ (u32)event.window.data1, (u32)event.window.data2 });
					return true;
				}
			}
		}
		
		return true;
	}
}