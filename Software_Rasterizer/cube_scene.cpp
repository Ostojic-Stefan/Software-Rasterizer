#include "cube_scene.hpp"

struct Vertex
{
	math::vec3 pos;
	math::vec3 color;
};

static std::vector<Vertex> vertexData = {
	Vertex{ { -0.5f, -0.5f, 0.f }, { 1.0f, 0.f, 0.f } },
	Vertex{ {  0.5f, -0.5f, 0.f }, { 0.0f, 1.f, 0.f } },
	Vertex{ {  0.0f,  0.5f, 0.f }, { 0.0f, 0.f, 1.f } },
};

cube_plain_scene::cube_plain_scene(rnd::framebuffer& fb)
	:
	_fb(fb),
	_generic_renderer(fb)

{
	vboId = _generic_renderer.CreateVertexBuffer(vertexData.data(), sizeof(Vertex));
	_generic_renderer.BindVertexBuffer(vboId);
	_generic_renderer.SetVertexAttribute(vboId, { AttribType::Float, 3, offsetof(Vertex, pos), 0 });
	_generic_renderer.SetVertexAttribute(vboId, { AttribType::Float, 3, offsetof(Vertex, color), 1 });
}

void cube_plain_scene::update(rnd::f32 dt)
{
	LOG("{}", 1.f / dt);
}


void cube_plain_scene::render()
{
	_fb.clear_color(rnd::dark_gray);

	float total_time = SDL_GetTicks() / 1000.f;
	_shader_program.vs.total_time = total_time;
	_generic_renderer.BindShaderProgram(&_shader_program);

	_generic_renderer.Draw(vertexData.size());
}
