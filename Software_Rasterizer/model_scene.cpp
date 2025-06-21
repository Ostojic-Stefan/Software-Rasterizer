#include "model_scene.hpp"

#include "input.hpp"

mode_scene::mode_scene(rnd::framebuffer& fb)
	:
	_fb(fb),
	_generic_renderer(fb),
	_camera(5.f),
	_cam_ctrl(_camera),
	the_model("../assets/models/nanosuit.obj")
{
	_generic_renderer.SetViewport({ 0, 0 }, { 800, 600 });
	_generic_renderer.BindShaderProgram(&_shader_program);


	_point_light.position = { 5.f, 0.f, 0.f };
	_point_light.ambient = { 0.05f, 0.05f, 0.05f };
	_point_light.diffuse = { 1.f, 1.f, 1.f };

	_point_light.att_const = 0.1f;
	_point_light.att_linear = 0.1f;
	_point_light.att_quad = 0.0032f;


	for (gfx::mesh& mesh : the_model.meshes)
	{
		mesh.vboid = _generic_renderer.CreateVertexBuffer(mesh.vertices.data(), sizeof(gfx::vertex));
		_generic_renderer.BindVertexBuffer(mesh.vboid);

		_generic_renderer.SetVertexAttribute({ AttribType::Float, 3, offsetof(gfx::vertex, position), 0 });
		_generic_renderer.SetVertexAttribute({ AttribType::Float, 3, offsetof(gfx::vertex, normal), 1 });
		_generic_renderer.SetVertexAttribute({ AttribType::Float, 2, offsetof(gfx::vertex, tex_coords), 2 });
		_generic_renderer.SetVertexAttribute({ AttribType::Float, 3, offsetof(gfx::vertex, tangent), 3 });
		_generic_renderer.SetVertexAttribute({ AttribType::Float, 3, offsetof(gfx::vertex, bitangent), 4 });

		mesh.iboid = _generic_renderer.CreateIndexBuffer(mesh.indices.data(), mesh.indices.size());
		_generic_renderer.BindIndexBuffer(mesh.iboid);
	}
}

static rnd::f32 total_time = 0.f;

void mode_scene::update(rnd::f32 dt)
{
	total_time += dt;
	_cam_ctrl.update(dt);

	static constexpr float light_dist = 15.f;
	_point_light.position = { light_dist * std::cos(total_time * .5f), 0.f, light_dist * std::sin(total_time * .5f) };

	_shader_program.fs.bind_view_direction(_camera.get_position());

	//_shader_program.vs.total_time += dt;
}

enum class REND_TYPE : int { NON_MT, MT, COUNT };

static REND_TYPE rend_type = REND_TYPE::MT;

void mode_scene::render()
{
	_fb.clear_color(rnd::dark_gray);
	_fb.clear_depth();
	_shader_program.vs.bindViewMatrix(_camera.get_view_matrix());
	_shader_program.fs.bind_point_light(_point_light);

	for (gfx::mesh& mesh : the_model.meshes)
	{
		_generic_renderer.BindVertexBuffer(mesh.vboid);
		_generic_renderer.BindIndexBuffer(mesh.iboid);

		if (rnd::input::is_key_pressed(rnd::input::key_code::KP_1))
		{
			rend_type = REND_TYPE::NON_MT;
		}

		else if (rnd::input::is_key_pressed(rnd::input::key_code::KP_2))
		{
			rend_type = REND_TYPE::MT;
		}

		switch (rend_type)	
		{
		case REND_TYPE::MT:
			_generic_renderer.DrawIndexedBin(mesh.indices.size());
			break;
		case REND_TYPE::NON_MT:
			_generic_renderer.DrawIndexed(mesh.indices.size());
			break;
		}
	}
}

////////// SHADERS //////////

VSOutput model_shader_program::vertex_shader::operator()(const VSInput& in) const
{
	math::vec3 pos = in.Get<math::vec3>(0);
	math::vec3 normal = in.Get<math::vec3>(1);
	math::vec2 tc = in.Get<math::vec2>(2);

	VSOutput out;
	math::mat4 model = math::mat4::translate({ 0.f, 0.f, total_time }) * math::mat4::scale(1.5f);
	out.Position = _projection * _view * model * math::vec4{ pos, 1.0f };
	normal = model * normal;

	out.setVarying<math::vec3>(0, pos);
	out.setVarying<math::vec3>(1, normal);
	out.setVarying<math::vec2>(2, tc);

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

void model_shader_program::fragment_shader::bind_point_light(const point_light& p_light)
{
	this->p_light = p_light;
}

void model_shader_program::fragment_shader::bind_view_direction(const math::vec3& cam_pos)
{
	this->cam_pos = cam_pos;
}

math::vec4 model_shader_program::fragment_shader::operator()(const VSOutput& vsout) const
{
	math::vec3 frag_pos = vsout.getVarying<math::vec3>(0);
	math::vec3 normal = vsout.getVarying<math::vec3>(1);
	math::vec2 tc = vsout.getVarying<math::vec2>(2);

	math::vec3 object_color(1.f);

	normal = math::normalize(normal);

	rnd::f32 ambientStrength = 0.1f;
	math::vec3 ambient = ambientStrength * p_light.diffuse;

	// diffuse
	math::vec3 norm = math::normalize(normal);
	math::vec3 lightDir = normalize(p_light.position - frag_pos);
	rnd::f32 diff = std::max(math::dot(norm, lightDir), 0.0f);
	math::vec3 diffuse = diff * p_light.diffuse;

	// specular
	rnd::f32 specularStrength = 0.5;
	math::vec3 viewDir = math::normalize(cam_pos - frag_pos);
	math::vec3 reflectDir = reflect(-lightDir, norm);
	rnd::f32 spec = pow(std::max(math::dot(viewDir, reflectDir), 0.0f), 5.f);
	math::vec3 specular = specularStrength * spec * p_light.diffuse;

	rnd::f32 distance = math::length(p_light.position - frag_pos);
	rnd::f32 attenuation = 1.f / (p_light.att_const + p_light.att_linear * distance + p_light.att_quad * distance * distance);

	math::vec3 result = (ambient + diffuse + specular) * object_color * attenuation;

	math::vec4 color(result, 1.f);

	return color;
}

