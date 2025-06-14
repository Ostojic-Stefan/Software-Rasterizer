#include "cube_scene.hpp"

cube_plain_scene::cube_plain_scene(rnd::framebuffer& fb)
	:
	_fb(fb),
	_renderer(fb),


	_camera(5.f),
	_cam_ctrl(_camera),

	_cube_mesh(gfx::get_cube_mesh(1.f))
{
	_renderer.bind_viewport(_viewport);

	_vs.bind_model_matrix(math::mat4::identity());
	_vs.bind_projection_matrix(_projection);
}

void cube_plain_scene::update(rnd::f32 dt)
{
	LOG("{}", 1.f / dt);
}

void cube_plain_scene::render()
{
	_fb.clear_color(rnd::dark_gray);
	_fb.clear_depth();

	_vs.bind_view_matrix(_camera.get_view_matrix());

	_renderer.bind_vertex_shader(&_vs);
	_renderer.bind_fragment_shader(&_fs);

	_renderer.bind_mesh(_cube_mesh);
	_renderer.draw();
}
