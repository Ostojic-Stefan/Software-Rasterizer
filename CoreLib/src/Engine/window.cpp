#include "pch.h"
#include "window.hpp"

namespace rnd
{
	sdl_window::sdl_window(const window_options& opts)
		:
		width{ opts.width },
		height{ opts.height }
	{
		SDL_InitSubSystem(SDL_INIT_EVENTS | SDL_INIT_VIDEO);

		SDL_DisplayMode display_mode;
		SDL_GetCurrentDisplayMode(0, &display_mode);

		i32 x = SDL_WINDOWPOS_UNDEFINED;
		i32 y = SDL_WINDOWPOS_UNDEFINED;

		i32 flags = SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE;

		window_handle = SDL_CreateWindow(opts.name.data(), x, y, opts.width, opts.height, flags);

		ASSERT(window_handle, "Failed to initialize SDL window. {}", SDL_GetError());

		renderer = SDL_CreateRenderer(window_handle, -1, SDL_RENDERER_ACCELERATED);
		ASSERT(renderer, "Failed to initialize SDL renderer. {}", SDL_GetError());

		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, width, height);
		ASSERT(texture, "Failed to initialize SDL texture. {}", SDL_GetError());
	}

	sdl_window::~sdl_window()
	{
		SDL_DestroyWindow(window_handle);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyTexture(texture);
	}

	void sdl_window::show()
	{
		SDL_ShowWindow(window_handle);
	}

	void sdl_window::hide()
	{
		SDL_HideWindow(window_handle);
	}

	rnd::u16 sdl_window::get_width() const
	{
		return width;
	}

	rnd::u16 sdl_window::get_height() const
	{
		return height;
	}

	void sdl_window::resize(rnd::i16 width, rnd::i16 height)
	{
		SDL_DestroyTexture(texture);
		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, width, height);

		this->width = width;
		this->height = height;
	}

	void* sdl_window::get_native_handle() const
	{
		return window_handle;
	}

	void sdl_window::display_framebuffer(const rnd::framebuffer& fb)
	{
		SDL_UpdateTexture(texture, NULL, fb.get_color_buffer(), fb.get_width() * sizeof(rnd::color));
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

}

