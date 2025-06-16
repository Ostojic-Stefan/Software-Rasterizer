#include "application.hpp"

#include <algorithm>

application::application()
	:
	fb(800, 600),
	_cube_scene(fb)
{
	platform::initialize({
		.title = "Software Rasterizer",
		.width = 800,
		.height = 600,
	});

	rnd::input::init();
}

application::~application()
{
	platform::shutdown();
}

void application::run()
{
	rnd::timer timer;
	
	while (running)
	{
		rnd::input::update();

		platform::process_events();

		dt = timer.get_elapsed_s();
		total_time += dt;

		update(dt);
		render();

		platform::display_framebuffer(fb);
	}
}

void application::update(rnd::f32 dt)
{
	if (rnd::input::is_key_pressed(rnd::input::key_code::A))
	{
		LOG("key pressed!");
	}

	if (rnd::input::is_mouse_pressed(rnd::input::mouse_btn::left))
	{
		LOG("left pressed");
	}

	auto pos = rnd::input::get_mouse_pos();
	// LOG("x = {}, y = {}", pos.x, pos.y);

	//LOG("{}", dt);
	//LOG("{}", camera.get_position().x);
	_cube_scene.update(dt);
}


void application::render()
{
	fb.clear_color(rnd::dark_gray);
	_cube_scene.render();
}
