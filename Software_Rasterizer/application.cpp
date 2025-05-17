#include "application.hpp"

#include <algorithm>

application::application()
	:
	window({ "Software rendering", 800, 600 }),
	fb(800, 600),
	object_renderer(fb),
	light_renderer(fb),
	camera(1.0f),
	cam_ctrl(camera),
	move_cam(math::vec3(0.0f, 0.0f, 3.0f))
{
	event_handler.bind_key_press_cb([this](const rnd::key_event& ev) {
		if (ev.code == rnd::key_code::ESCAPE)
			running = false;

		cam_ctrl.key_pressed_handler(ev);

		//if (ev.code == rnd::key_code::R) {
		//	light_pos.z -= 0.1f;
		//}

		//if (ev.code == rnd::key_code::F) {
		//	light_pos.z += 0.1f;
		//}

		//if (ev.code == rnd::key_code::J) {
		//	light_pos.x -= 0.1f;
		//}

		//if (ev.code == rnd::key_code::L) {
		//	light_pos.x += 0.1f;
		//}

		//if (ev.code == rnd::key_code::I) {
		//	light_pos.y += 0.1f;
		//}

		//if (ev.code == rnd::key_code::K) {
		//	light_pos.y -= 0.1f;
		//}

		//if (ev.code == rnd::key_code::W) {
		//	move_cam.process_keyboard(FORWARD, dt);
		//}

		//if (ev.code == rnd::key_code::S) {
		//	move_cam.process_keyboard(BACKWARD, dt);
		//}

		//if (ev.code == rnd::key_code::A) {
		//	move_cam.process_keyboard(LEFT, dt);
		//}

		//if (ev.code == rnd::key_code::D) {
		//	move_cam.process_keyboard(RIGHT, dt);
		//}

		if (ev.code == rnd::key_code::UP) {
			mip_level += 1;
			mip_level = std::min(10, mip_level);
		}

		if (ev.code == rnd::key_code::DOWN) {
			mip_level -= 1;
			mip_level = std::max(0, mip_level);
		}
	});

	event_handler.bind_mouse_move_cb([this](const rnd::mouse_move_event& ev) {
		cam_ctrl.mouse_move_handler(ev);

		//rnd::f32 xpos = ev.pos.x;
		//rnd::f32 ypos = ev.pos.y;

		//if (firstMouse)
		//{
		//	lastX = xpos;
		//	lastY = ypos;
		//	firstMouse = false;
		//}

		//float xoffset = xpos - lastX;
		//float yoffset = lastY - ypos;

		//lastX = xpos;
		//lastY = ypos;

		//move_cam.process_mouse_movement(xoffset, yoffset);
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

	//diffuse_tex = gfx::texture::from_file("../assets/pikuma.png");
	diffuse_tex = gfx::texture::from_file("../assets/brick_1024.jpg");
	//diffuse_tex = gfx::texture::from_file("../assets/drone.png");
	specular_tex = gfx::texture::from_file("../assets/container2_specular.png");

	diffuse_tex.generate_mipmaps();
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
		dt = timer.get_elapsed_s();
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
	//LOG("{}", dt);
}

void application::render()
{
	fb.clear_color(rnd::dark_gray);
	fb.clear_depth();

	object_renderer.bind_viewport(viewport);
	light_renderer.bind_viewport(viewport);

	vertex_shader vs1;
	fragment_shader fs1;

	//fs1.add_point_light(point_light{
	//	.position = light_pos1,
	//	.color = math::vec3(1.0f),
	//	.att_quad = 0.05f,
	//	.att_lin = 0.09f,
	//	.att_const = 0.032f,
	//});

	//fs1.add_point_light(point_light{
	//	.position = light_pos2,
	//	.color = math::vec3(1.0f),
	//	.att_quad = 0.05f,
	//	.att_lin = 0.09f,
	//	.att_const = 0.032f,
	//});

	//diffuse_tex.set_mip_level(mip_level);

	object_renderer.bind_texture(&diffuse_tex);

	fs1.bind_diffuse_texture(&diffuse_tex);

	//fs1.bind_specular_texture(&specular_tex);


	//render_mesh(cube_mesh, vs1, fs1,
	//	math::mat4::translate(math::vec3(4.0f, 0.0f, 0.0f)) *
	//	math::mat4::scale(1.0f)
	//);

	render_mesh(triangle_mesh, vs1, fs1,
		math::mat4::translate(math::vec3(0.0f, 0.0f, 0.0f)) *
		math::mat4::scale(1.0f)
	);

	//render_mesh(cube_mesh, vs1, fs1,
	//	math::mat4::translate(math::vec3(-4.0f, 0.0f, 0.0f)) *
	//	math::mat4::scale(1.0f)
	//);

	//render_mesh(plane_mesh, vs1, fs1, 
	//	math::mat4::translate(math::vec3(0.0f, -1.01f, 0.0f)) *
	//	//math::mat4::rotation_x(-math::pi32 / 2.0f) *
	//	math::mat4::scale(5.f)
	//);

	//light_pos1.x = 2.0f * std::cos(1.0f * total_time);
	//light_pos1.z = 2.0f * std::sin(1.0f * total_time);
	//light_pos2.y = 2.0f * std::cos(1.0f * total_time);
	//light_pos2.z = 2.0f * std::sin(1.0f * total_time);

	//render_point_light(
	//	math::mat4::translate(light_pos1) *
	//	math::mat4::scale(0.2f)
	//);

	//render_point_light(
	//	math::mat4::translate(light_pos2) *
	//	math::mat4::scale(0.2f)
	//);
}

void application::render_mesh(const gfx::mesh& mesh, vertex_shader& vs, fragment_shader& fs,
	const math::mat4& model)
{
	object_renderer.bind_vertex_shader(&vs);
	object_renderer.bind_fragment_shader(&fs);

	vs.bind_view_matrix(camera.get_view_matrix());

	vs.bind_projection_matrix(proj);
	vs.bind_model_matrix(model);

	fs.bind_camera_position(move_cam.Position);

	object_renderer.bind_mesh(mesh);
	object_renderer.draw();
}

void application::render_point_light(const math::mat4& model)
{
	fragment_shader_point_light fs;
	vertex_shader_point_light vs;

	light_renderer.bind_vertex_shader(&vs);
	light_renderer.bind_fragment_shader(&fs);

	vs.bind_view_matrix(camera.get_view_matrix());
	vs.bind_projection_matrix(proj);
	vs.bind_model_matrix(model);

	light_renderer.bind_mesh(sphere_mesh);
	light_renderer.draw();
}
