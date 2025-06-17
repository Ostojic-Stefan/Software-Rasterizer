#include "cube_scene.hpp"

struct Vertex
{
	math::vec3 pos;
	math::vec3 color;
	math::vec2 tc;
};

//static std::vector<Vertex> vertexData = {
//	Vertex{ { -0.5f, -0.5f, 0.f }, { 1.0f, 0.f, 0.f }, { 0.f, 0.f } },
//	Vertex{ {  0.5f, -0.5f, 0.f }, { 0.0f, 1.f, 0.f }, { 1.f, 0.f } },
//	Vertex{ {  0.5f,  0.5f, 0.f }, { 0.0f, 0.f, 1.f }, { 1.f, 1.f } },
//	Vertex{ { -0.5f,  0.5f, 0.f }, { 1.0f, 1.f, 1.f }, { 0.f, 1.f } },
//};
//
//static std::vector<uint16_t> indices = { 0,1,2, 0,2,3 };

// 6 faces × 4 verts each = 24 vertices
static const std::vector<Vertex> cubeVertices = {
	// +X face (right) – red
	{{ 0.5f, -0.5f, -0.5f}, {1, 0, 0}, {0, 0}},
	{{ 0.5f,  0.5f, -0.5f}, {1, 0, 0}, {1, 0}},
	{{ 0.5f,  0.5f,  0.5f}, {1, 0, 0}, {1, 1}},
	{{ 0.5f, -0.5f,  0.5f}, {1, 0, 0}, {0, 1}},

	// -X face (left) – green
	{{-0.5f, -0.5f,  0.5f}, {0, 1, 0}, {0, 0}},
	{{-0.5f,  0.5f,  0.5f}, {0, 1, 0}, {1, 0}},
	{{-0.5f,  0.5f, -0.5f}, {0, 1, 0}, {1, 1}},
	{{-0.5f, -0.5f, -0.5f}, {0, 1, 0}, {0, 1}},

	// +Y face (top) – blue
	{{-0.5f,  0.5f, -0.5f}, {0, 0, 1}, {0, 0}},
	{{-0.5f,  0.5f,  0.5f}, {0, 0, 1}, {1, 0}},
	{{ 0.5f,  0.5f,  0.5f}, {0, 0, 1}, {1, 1}},
	{{ 0.5f,  0.5f, -0.5f}, {0, 0, 1}, {0, 1}},

	// -Y face (bottom) – yellow
	{{-0.5f, -0.5f,  0.5f}, {1, 1, 0}, {0, 0}},
	{{-0.5f, -0.5f, -0.5f}, {1, 1, 0}, {1, 0}},
	{{ 0.5f, -0.5f, -0.5f}, {1, 1, 0}, {1, 1}},
	{{ 0.5f, -0.5f,  0.5f}, {1, 1, 0}, {0, 1}},

	// +Z face (front) – magenta
	{{ 0.5f, -0.5f,  0.5f}, {1, 0, 1}, {0, 0}},
	{{ 0.5f,  0.5f,  0.5f}, {1, 0, 1}, {1, 0}},
	{{-0.5f,  0.5f,  0.5f}, {1, 0, 1}, {1, 1}},
	{{-0.5f, -0.5f,  0.5f}, {1, 0, 1}, {0, 1}},

	// -Z face (back) – cyan
	{{-0.5f, -0.5f, -0.5f}, {0, 1, 1}, {0, 0}},
	{{-0.5f,  0.5f, -0.5f}, {0, 1, 1}, {1, 0}},
	{{ 0.5f,  0.5f, -0.5f}, {0, 1, 1}, {1, 1}},
	{{ 0.5f, -0.5f, -0.5f}, {0, 1, 1}, {0, 1}},
};

// 6 faces × 2 triangles × 3 indices = 36 indices
static const std::vector<uint16_t> cubeIndices = {
	0,  1,  2,   2,  3,  0,    // +X
	4,  5,  6,   6,  7,  4,    // -X
	8,  9, 10,  10, 11,  8,    // +Y
	12, 13, 14,  14, 15, 12,   // -Y
	16, 17, 18,  18, 19, 16,   // +Z
	20, 21, 22,  22, 23, 20,   // -Z
};

static inline std::vector<Vertex> BuildVertexDataFromMesh(const gfx::mesh& mesh)
{
	std::vector<Vertex> result;
	size_t size = mesh.positions.size();

	result.reserve(size);

	for (int i = 0; i < size; ++i)
	{
		Vertex v;
		v.pos = mesh.positions[i];
		v.color = mesh.colors[i];
		v.tc = mesh.tex_coords[i];

		result.push_back(v);
	}

	return result;
}

cube_plain_scene::cube_plain_scene(rnd::framebuffer& fb)
	:
	_fb(fb),
	_generic_renderer(fb)

{

	vboId = _generic_renderer.CreateVertexBuffer(cubeVertices.data(), sizeof(Vertex));
	//vboId = _generic_renderer.CreateVertexBuffer(vertexData.data(), sizeof(Vertex));

	_generic_renderer.BindVertexBuffer(vboId);
	_generic_renderer.SetVertexAttribute(vboId, { AttribType::Float, 3, offsetof(Vertex, pos), 0 });
	_generic_renderer.SetVertexAttribute(vboId, { AttribType::Float, 3, offsetof(Vertex, color), 1 });
	_generic_renderer.SetVertexAttribute(vboId, { AttribType::Float, 2, offsetof(Vertex, tc), 2 });

	iboId = _generic_renderer.CreateIndexBuffer(cubeIndices.data(), cubeIndices.size());
	//iboId = _generic_renderer.CreateIndexBuffer(indices.data(), indices.size());

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
	//_generic_renderer.DrawIndexed(cube_mesh.indices.size());
	_generic_renderer.DrawIndexed(cubeIndices.size());
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
	
	math::vec4 col = surf.sample(tc.x, tc.y) * math::vec4(color, 1.f);
	//math::vec4 col = surf.sample(tc.x, tc.y);
	return col;
}

VSOutput BasicShaderProgram::VertexShader::operator()(const VsInput& in) const
{
	math::vec3 pos = in.Get<math::vec3>(0);
	math::vec3 color = in.Get<math::vec3>(1);
	math::vec2 tc = in.Get<math::vec2>(2);
	
	VSOutput out;
	math::mat4 model =
		math::mat4::translate({ 0.f, 0.f, -2.f }) *
		math::mat4::rotation_y(total_time) *
		math::mat4::rotation_x(total_time) *
		math::mat4::rotation_z(total_time);

	out.Position = _projection * model * math::vec4{ pos, 1.0f };
	out.setVarying<math::vec3>(0, color);
	out.setVarying<math::vec2>(1, tc);
	
	return out;
}
