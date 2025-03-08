#pragma once

#include <string_view>
#include <memory>
#include <SDL2/SDL.h>

#include "types.hpp"
#include "frame_buffer.hpp"

namespace rnd
{
	struct window_options
	{
		window_options(std::string_view name, rnd::u16 width, rnd::u16 height)
			: name(name), width(width), height(height) {}
		std::string_view name;
		rnd::u16 width;
		rnd::u16 height;
	};


	struct i_window
	{
		virtual ~i_window() = default;

		virtual void show() = 0;
		virtual void hide() = 0;
		virtual rnd::u16 get_width() const = 0;
		virtual rnd::u16 get_height() const = 0;

		virtual void resize(rnd::i16 width, rnd::i16 height) = 0;

		virtual void* get_native_handle() const = 0;

		virtual void display_framebuffer(const rnd::framebuffer& fb) = 0;
	};

	struct sdl_window : public i_window
	{
		sdl_window(const window_options& opts);

		~sdl_window() override;

		sdl_window(const sdl_window&) = delete;
		sdl_window& operator=(const sdl_window&) = delete;

		void show() override;
		void hide() override;

		rnd::u16 get_width() const override;
		rnd::u16 get_height() const override;

		void resize(rnd::i16 width, rnd::i16 height) override;
		
		void* get_native_handle() const override;

		virtual void display_framebuffer(const rnd::framebuffer& fb) override;
	private:
		rnd::u16 width;
		rnd::u16 height;

		SDL_Window* window_handle = nullptr;

		SDL_Renderer* renderer = nullptr;
		SDL_Texture* texture = nullptr;
	};

	static inline i_window* create_window(const rnd::window_options& opts)
	{
		return new sdl_window (opts);
	}
}