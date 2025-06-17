#include "cube_scene.hpp"

struct Vertex
{
	math::vec3 pos;
	math::vec3 color;
	math::vec2 tc;
};

static std::vector<Vertex> vertexData = {
	Vertex{ { -0.5f, -0.5f, 0.f }, { 1.0f, 0.f, 0.f }, { 0.f, 0.f } },
	Vertex{ {  0.5f, -0.5f, 0.f }, { 0.0f, 1.f, 0.f }, { 1.f, 0.f } },
	Vertex{ {  0.5f,  0.5f, 0.f }, { 0.0f, 0.f, 1.f }, { 1.f, 1.f } },
	Vertex{ { -0.5f,  0.5f, 0.f }, { 1.0f, 1.f, 1.f }, { 0.f, 1.f } },
};

static std::vector<uint16_t> indices = { 0,1,2, 0,2,3 };

cube_plain_scene::cube_plain_scene(rnd::framebuffer& fb)
	:
	_fb(fb),
	_generic_renderer(fb)

{
	vboId = _generic_renderer.CreateVertexBuffer(vertexData.data(), sizeof(Vertex));
	_generic_renderer.BindVertexBuffer(vboId);
	_generic_renderer.SetVertexAttribute(vboId, { AttribType::Float, 3, offsetof(Vertex, pos), 0 });
	_generic_renderer.SetVertexAttribute(vboId, { AttribType::Float, 3, offsetof(Vertex, color), 1 });
	_generic_renderer.SetVertexAttribute(vboId, { AttribType::Float, 2, offsetof(Vertex, tc), 2 });

	iboId = _generic_renderer.CreateIndexBuffer(indices.data(), indices.size());
	_generic_renderer.BindIndexBuffer(iboId);
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

	//_generic_renderer.Draw(vertexData.size());
	_generic_renderer.DrawIndexed(6);
}

/////////////////////////
// SHADER
/////////////////////////

BasicShaderProgram::FragmentShader::FragmentShader()
{
	//surf = gfx::surface::from_file("../assets/container2.png");
	surf = gfx::surface::from_file("../assets/checker.jpg");
}

math::vec4 BasicShaderProgram::FragmentShader::operator()(const VSOutput& vsout) const
{
	math::vec3 color = vsout.getVarying<math::vec3>(0);
	math::vec2 tc = vsout.getVarying<math::vec2>(1);
	
	math::vec4 col = surf.sample(tc.x, tc.y);
	return col;
	//return math::vec4{ color.x, color.y, color.z, 1.0f };
	//return math::vec4{ tc.x, tc.y, 0.f, 1.0f };
}

VSOutput BasicShaderProgram::VertexShader::operator()(const VsInput& in) const
{
	math::vec3 pos = in.Get<math::vec3>(0);
	math::vec3 color = in.Get<math::vec3>(1);
	math::vec2 tc = in.Get<math::vec2>(2);
	
	VSOutput out;
	math::mat4 model = 
		math::mat4::translate({0.f, 0.f, -1.f}) *
		math::mat4::rotation_y(total_time);

	out.Position = _projection * model * math::vec4{ pos, 1.0f };
	out.setVarying<math::vec3>(0, color);
	out.setVarying<math::vec2>(1, tc);
	
	return out;
}
