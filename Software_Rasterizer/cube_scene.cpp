#include "cube_scene.hpp"

cube_plain_scene::cube_plain_scene(rnd::framebuffer& fb)
	:
	_fb(fb),
	_generic_renderer(fb)

{
	//std::vector<rnd::u16> indices = { 0, 1, 2, 0, 2, 3 };

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

	//_the_renderer.bind_index_buffer(indices);
	//_the_renderer.bind_vertex_buffer(vertices);
}

void cube_plain_scene::update(rnd::f32 dt)
{
	LOG("{}", 1.f / dt);
}

void cube_plain_scene::render()
{
	struct Vertex 
	{ 
		math::vec3 pos;
		math::vec3 color;
	};

	std::vector<Vertex> vertexData = {
		Vertex{ { -0.5f, -0.5f, 0.f }, { 1.0f, 0.f, 0.f } },
		Vertex{ {  0.5f, -0.5f, 0.f }, { 0.0f, 1.f, 0.f } },
		Vertex{ {  0.0f,  0.5f, 0.f }, { 0.0f, 0.f, 1.f } },
	};

	_fb.clear_color(rnd::dark_gray);
	int vboId = _generic_renderer.CreateVertexBuffer(vertexData.data(), sizeof(Vertex));
	_generic_renderer.BindVertexBuffer(vboId);
	_generic_renderer.SetVertexAttribute(vboId, { AttribType::Float, 3, offsetof(Vertex, pos), 0 });
	_generic_renderer.SetVertexAttribute(vboId, { AttribType::Float, 3, offsetof(Vertex, color), 1 });

	float total_time = SDL_GetTicks() / 1000.f;

	_generic_renderer.BindVertexShader([total_time](const VsInput& in) {
		math::vec3 pos = in.Get<math::vec3>(0);
		math::vec3 color = in.Get<math::vec3>(1);

		VSOutput out;
		out.Position = math::mat4::rotation_y(total_time) * math::vec4{ pos.x, pos.y, pos.z, 1.0f };
		out.setVarying<math::vec3>(0, color);

		return out;
	});

	_generic_renderer.BindFragmentShader([](const VSOutput& output) {
		GenericValue v = output.getVarying(0);

		// TODO: no hard-code.
		math::vec3 color = { v.vals[0],v.vals[1] ,v.vals[2] };

		return math::vec4{color.x, color.y, color.z, 1.0f};
	});


	_generic_renderer.Draw(vertexData.size());
	//_fb.clear_depth();

	//_vs.bind_view_matrix(_camera.get_view_matrix());

	//_renderer.bind_vertex_shader(&_vs);
	//_renderer.bind_fragment_shader(&_fs);

	//_renderer.bind_mesh(_cube_mesh);
	//_renderer.draw();
}
