#include "application.hpp"

#include <algorithm>

application::application()
	:
	fb(800, 600),
	_cube_scene(fb),
	_model_scene(fb)
	//renderer(fb)
{
	platform::initialize({
		.title = "Software Rasterizer",
		.width = 800,
		.height = 600,
	});

	rnd::input::init();

	//std::vector<rnd::u16> indices = { 0, 1, 2 };

	//std::vector<shader_program::vertex_input> vertices = 
	//{
	//	shader_program::vertex_input{
	//		.position = {-0.5f, -0.5f, 0.f},
	//		.color = {1.0f, 0.f, 0.f, 1.f},
	//	},
	//	shader_program::vertex_input{
	//		.position = {0.5f, -0.5f, 0.f},
	//		.color = {0.f, 1.f, 0.f, 1.f},
	//	},
	//	shader_program::vertex_input{
	//		.position = {0.5f, 0.5f, 0.f},
	//		.color = {0.f, 0.f, 1.f, 1.f},
	//	},
	//	shader_program::vertex_input{
	//		.position = {-0.5f, 0.5f, 0.f},
	//		.color = {1.f, 1.f, 1.f, 1.f},
	//	}
	//};

	//renderer.bind_index_buffer(indices);
	//renderer.bind_vertex_buffer(vertices);
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

	LOG("{}", 1.f / dt);
	//LOG("{}", camera.get_position().x);
	//_cube_scene.update(dt);
	_model_scene.update(dt);
}


void application::render()
{
	//fb.clear_color(rnd::dark_gray);
	//renderer.render_indexed();
	//_cube_scene.render();
	_model_scene.render();
}
