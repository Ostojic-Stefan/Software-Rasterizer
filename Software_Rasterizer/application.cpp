#include "application.hpp"

application::application()
	:
	window({ "Software rendering", 800, 600 }),
	fb(800, 600),
	renderer(fb),
	camera(5.f),
	cam_ctrl(camera)
{
	event_handler.bind_key_press_cb([this](const rnd::key_event& ev) {
		if (ev.code == rnd::key_code::ESCAPE)
			running = false;

		cam_ctrl.key_pressed_handler(ev);
	});

	event_handler.bind_mouse_move_cb([this](const rnd::mouse_move_event& ev) {
		cam_ctrl.mouse_move_handler(ev);
	});

	event_handler.bind_mouse_pressed_cb([this](const rnd::mouse_button_event& ev) {
		cam_ctrl.mouse_button_handler(ev);
	});

	event_handler.bind_window_resized_cb([this](const rnd::window_resize_event& ev) {
		window.resize(ev.width, ev.height);
		fb.reset(ev.width, ev.height);
		viewport.xmax = ev.width;
		viewport.ymax = ev.height;
	});

	tex = gfx::texture::from_file("../assets/brick_1024.jpg");

	fs.bind_texture(&tex);
}

application::~application()
{
}

void application::run()
{
	window.show();

	rnd::timer timer;
	
	while (running)
	{
		rnd::f32 dt = timer.get_elapsed_s();
		total_time += dt;

		if (!rnd::poll_events(event_handler))
			running = false;

		update(dt);
		render();

		window.display_framebuffer(fb);
	}
}

void application::update(rnd::f32 dt)
{
	LOG("{}", dt);
}


void application::render()
{
	fb.clear_color(rnd::dark_gray);
	fb.clear_depth();
	
	vs.bind_model_matrix(
		math::mat4::translate({ 0.f, 0.f, 0.f })
	);
	vs.bind_view_matrix(camera.get_view_matrix({ 0.f, 0.f, 0.f }));
	vs.bind_projection_matrix(math::mat4::perspective(0.1f, 100.f, math::pi32 / 2.f, 800.f / 600.f));

	fs.bind_camera_position(camera.get_position());

	renderer.bind_vertex_shader(vs);
	renderer.bind_fragment_shader(fs);

	renderer.bind_viewport(viewport);
	renderer.bind_mesh(mesh);

	renderer.draw();
}
