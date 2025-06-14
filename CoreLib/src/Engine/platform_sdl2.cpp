#include "pch.h"

#include "platform.hpp"
#include "event.hpp"
#include <SDL2/SDL.h>

namespace platform
{
    struct internal_state
    {
        SDL_Window* window_handle = nullptr;
		SDL_Renderer* renderer = nullptr;
		SDL_Texture* texture = nullptr;
        struct config config;
    };

    static internal_state state;

    rnd::b8 initialize(const config& window_config)
    {
        rnd::i32 success = SDL_Init(SDL_INIT_VIDEO);
        if (success != 0)
        {
            //PXL_ERROR("Failed to initialize SDL");
            return false;
        }

        state.config = window_config;

        state.window_handle = SDL_CreateWindow(
            state.config.title.data(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            state.config.width, state.config.height, SDL_WINDOW_SHOWN
        );

		if (!state.window_handle)
		{
			//PXL_ERROR("Failed to create SDL Window");
			return false;
		}

		state.renderer = SDL_CreateRenderer(state.window_handle, -1, SDL_RENDERER_ACCELERATED);
		if (!state.renderer)
			return false;

		state.texture = SDL_CreateTexture(state.renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, state.config.width, state.config.height);
		if (!state.texture)
			return false;

        return true;
    }

    void process_events()
    {
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
			{
				rnd::event::Event close_event = {
					.type = rnd::event::EventType::AppQuit,
				};
				rnd::event::trigger_event(close_event);
			} break;
			case SDL_MOUSEMOTION:
			{
				math::vec2 pos = { (rnd::f32)event.motion.x, (rnd::f32)event.motion.y };

				rnd::event::MouseMoveData mouse_data { .pos = pos };
				rnd::event::Event mouse_move_event = {};
				mouse_move_event.type = rnd::event::EventType::MouseMove;
				mouse_move_event.data = { .mouse_move = mouse_data };

				rnd::event::trigger_event(mouse_move_event);
				rnd::input::process_mouse_move(pos);
				break;
			}
			case SDL_MOUSEWHEEL:
				rnd::input::process_mouse_wheel((rnd::f32)event.wheel.y);
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			{
				rnd::b8 is_pressed = event.type == SDL_MOUSEBUTTONDOWN;

				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
					rnd::input::process_mouse(rnd::input::mouse_btn::left, is_pressed);
					break;
				case SDL_BUTTON_RIGHT:
					rnd::input::process_mouse(rnd::input::mouse_btn::right, is_pressed);
					break;
				case SDL_BUTTON_MIDDLE:
					rnd::input::process_mouse(rnd::input::mouse_btn::middle, is_pressed);
					break;
				}
			}
			break;
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			{
				rnd::input::key_code code = (rnd::input::key_code)event.key.keysym.scancode;
				rnd::input::process_key(code, event.type == SDL_KEYDOWN);
			}
			break;
			// TODO: resizing events.
			case SDL_WINDOWEVENT:

				break;
			}
		}
    }

    void shutdown()
    {
		SDL_DestroyWindow(state.window_handle);
		SDL_DestroyRenderer(state.renderer);
		SDL_DestroyTexture(state.texture);
		SDL_Quit();
    }

	void display_framebuffer(const rnd::framebuffer& fb)
	{
		SDL_UpdateTexture(state.texture, NULL, fb.get_color_buffer(), fb.get_width() * sizeof(rnd::color));
		SDL_RenderClear(state.renderer);
		SDL_RenderCopy(state.renderer, state.texture, NULL, NULL);
		SDL_RenderPresent(state.renderer);
	}
}