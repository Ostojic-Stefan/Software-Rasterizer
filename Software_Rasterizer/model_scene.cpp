#include "model_scene.hpp"

//struct vertex
//{
//	math::vec3 pos;
//	math::vec3 normal;
//	math::vec2 tc;
//};
//
//static std::vector<vertex> vertices;
//static std::vector<rnd::u16> indices;

mode_scene::mode_scene(rnd::framebuffer& fb)
	:
	_fb(fb),
	_generic_renderer(fb),
	_camera(10.f),
	_cam_ctrl(_camera),
	the_model("../assets/models/nanosuit.obj")
{
	_generic_renderer.SetViewport({ 0, 0 }, { 800, 600 });
	_generic_renderer.BindShaderProgram(&_shader_program);

	for (gfx::mesh& mesh : the_model.meshes)
	{
		mesh.vboid = _generic_renderer.CreateVertexBuffer(mesh.vertices.data(), sizeof(gfx::vertex));
		_generic_renderer.BindVertexBuffer(mesh.vboid);

		_generic_renderer.SetVertexAttribute({ AttribType::Float, 3, offsetof(gfx::vertex, position), 0 });
		_generic_renderer.SetVertexAttribute({ AttribType::Float, 3, offsetof(gfx::vertex, normal), 1 });
		_generic_renderer.SetVertexAttribute({ AttribType::Float, 2, offsetof(gfx::vertex, tex_coords), 2 });
		_generic_renderer.SetVertexAttribute({ AttribType::Float, 3, offsetof(gfx::vertex, tangent), 3 });
		_generic_renderer.SetVertexAttribute({ AttribType::Float, 3, offsetof(gfx::vertex, bitangent), 4});

		mesh.iboid = _generic_renderer.CreateIndexBuffer(mesh.indices.data(), mesh.indices.size());
		_generic_renderer.BindIndexBuffer(mesh.iboid);
	}

	//auto vboId = _generic_renderer.CreateVertexBuffer(vertices.data(), sizeof(vertex));
	//_generic_renderer.BindVertexBuffer(vboId);

	//_generic_renderer.SetVertexAttribute({ AttribType::Float, 3, offsetof(vertex, pos), 0 });
	//_generic_renderer.SetVertexAttribute({ AttribType::Float, 3, offsetof(vertex, normal), 1 });
	//_generic_renderer.SetVertexAttribute({ AttribType::Float, 2, offsetof(vertex, tc), 2});

	//auto iboId = _generic_renderer.CreateIndexBuffer(indices.data(), indices.size());
	//_generic_renderer.BindIndexBuffer(iboId);

	//_generic_renderer.SetViewport({ 0, 0 }, { 800, 600 });
	//_generic_renderer.BindShaderProgram(&_shader_program);
}

void mode_scene::update(rnd::f32 dt)
{
	_cam_ctrl.update(dt);
}

void mode_scene::render()
{
	_fb.clear_color(rnd::dark_gray);
	_fb.clear_depth();
	_shader_program.vs.bindViewMatrix(_camera.get_view_matrix());

	for (gfx::mesh& mesh : the_model.meshes)
	{
		_generic_renderer.BindVertexBuffer(mesh.vboid);
		_generic_renderer.BindIndexBuffer(mesh.iboid);

		_generic_renderer.DrawIndexed(mesh.indices.size());
	}

	//_generic_renderer.DrawIndexed(indices.size());
}

////////// SHADERS //////////

VSOutput model_shader_program::vertex_shader::operator()(const VSInput& in) const
{
	math::vec3 pos = in.Get<math::vec3>(0);
	math::vec3 normal = in.Get<math::vec3>(1);
	math::vec2 tc = in.Get<math::vec2>(2);

	VSOutput out;
	math::mat4 model = math::mat4::identity() * math::mat4::scale(1.5f);
	out.Position = _projection * _view * model * math::vec4{ pos, 1.0f };
	normal = model * normal;

	out.setVarying<math::vec3>(0, normal);
	out.setVarying<math::vec2>(1, tc);

	return out;
}

void model_shader_program::vertex_shader::bindViewMatrix(const math::mat4& view)
{
	_view = view;
}

model_shader_program::fragment_shader::fragment_shader()
{
	surf = gfx::surface::from_file("../assets/checker.jpg");
}

math::vec4 model_shader_program::fragment_shader::operator()(const VSOutput& vsout) const
{
	math::vec3 normal = vsout.getVarying<math::vec3>(0);
	math::vec2 tc = vsout.getVarying<math::vec2>(1);

	//math::vec4 col = surf.sample(tc.x, tc.y);
	math::vec4 col(normal, 1.f);
	return col;
}


