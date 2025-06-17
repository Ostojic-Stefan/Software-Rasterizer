#pragma once

#include <Engine/engine.hpp>
#include "viewport.hpp"
#include "light.hpp"

struct vertex_input
{
	math::vec3 position;
	math::vec3 world_position;
	math::vec2 tex_coord;
	math::vec3 normal;
	math::vec4 color;
};

struct vertex_output
{
	math::vec4 position;
	math::vec3 world_position;
	math::vec2 tex_coord;
	math::vec3 normal;
	math::vec4 color;
};

enum class filtering
{
	nearest,
	linear,
};

struct vertex_shader
{
	inline void bind_model_matrix(const math::mat4& mat) { model = mat; }
	inline void bind_view_matrix(const math::mat4& mat) { view = mat; }
	inline void bind_projection_matrix(const math::mat4& mat) { projection = mat; }

	vertex_output operator()(vertex_input v)
	{
		vertex_output ret;

		ret.world_position = model * math::vec4(v.position, 1.0f);
		ret.position = projection * view * model * math::vec4(v.position, 1.0f);
		ret.normal = model * math::vec4(v.normal, 0.0);

		ret.tex_coord = v.tex_coord;
		ret.color = v.color;

		return ret;
	}

	void bind_time(rnd::f32 time) {
		this->time = time;
	}

private:
	math::mat4 model = math::mat4::identity();
	math::mat4 view = math::mat4::identity();
	math::mat4 projection = math::mat4::identity();

	rnd::f32 time;
};

struct vertex_shader_point_light
{
	inline void bind_model_matrix(const math::mat4& mat) { model = mat; }
	inline void bind_view_matrix(const math::mat4& mat) { view = mat; }
	inline void bind_projection_matrix(const math::mat4& mat) { projection = mat; }

	vertex_output operator()(vertex_input v)
	{
		vertex_output ret;

		ret.position = projection * view * model * math::vec4(v.position, 1.0f);
		
		return ret;
	}

private:
	math::mat4 model = math::mat4::identity();
	math::mat4 view = math::mat4::identity();
	math::mat4 projection = math::mat4::identity();
};

struct fragment_shader_point_light
{
	rnd::color operator()(const vertex_output& vert)
	{
		return rnd::white;
	}
};

struct fragment_shader
{
	fragment_shader()
	{
		point_lights.reserve(10);
		directional_lights.reserve(10);
	}

	rnd::color operator()(const vertex_output& vert)
	{
		math::vec3 normal = math::normalize(vert.normal);
		math::vec3 base_color(0.5f);

		math::vec3 diffuse_color = diffuse_tex->sample(vert.tex_coord.x, vert.tex_coord.y);
		//math::vec3 spec_color = spec_tex->sample(vert.tex_coord.x, vert.tex_coord.y);
		//math::vec3 cam_dir = math::normalize(cam_pos - vert.world_position);

		//math::vec3 result(0.f);

		//for (const point_light& pl : point_lights) 
		//{
		//    result = result + calc_point_light(pl, normal, vert.world_position, cam_dir, diffuse_color, spec_color);
		//}

		return rnd::to_color(diffuse_color);
	}

	math::vec3 calc_point_light(
		const point_light& light, const math::vec3& normal, const math::vec3& world_pos, const math::vec3& cam_dir,
		const math::vec3& diffuse_color, const math::vec3& spec_color
	)
	{
		math::vec3 light_dir = math::normalize(light.position - world_pos);

		// diffuse
		rnd::f32 diff = std::max(math::dot(normal, light_dir), 0.0f);
		// specular
		math::vec3 reflected = math::reflect(-light_dir, normal);
		rnd::f32 spec = std::pow(std::max(math::dot(cam_dir, reflected), 0.0f), 32.0f);
		// attenuation
		rnd::f32 dist = math::length(light.position - world_pos);
		rnd::f32 attenuation = 1.0f / (light.att_const + light.att_lin * dist + light.att_quad * (dist * dist));

		math::vec3 ambient = math::vec3(0.2f) * diffuse_color;
		math::vec3 diffuse = diff * diffuse_color;
		math::vec3 specular = spec * spec_color;

		ambient = ambient * attenuation;
		diffuse = diffuse * attenuation;
		specular = specular * attenuation;

		return ambient + diffuse + specular;
	}


	//rnd::color operator()(const vertex_output& vert)
	//{
	//    math::vec3 normal = math::normalize(vert.normal);
	//    math::vec3 base_color(0.5f);
	//    
	//    // ambient lighting
	//    math::vec3 diffuse_color = diffuse_tex->sample(vert.tex_coord.x, vert.tex_coord.y);
	//    math::vec3 spec_color = spec_tex->sample(vert.tex_coord.x, vert.tex_coord.y);

	//    math::vec3 ambient = math::vec3(0.2f) * diffuse_color;

	//    // diffuse lighting
	//    math::vec3 light_dir = math::normalize(math::vec3(std::sin(time), 0.0, -1.0));

	//    math::vec3 light_color = math::vec3(1.f, 1.f, 1.f);
	//    rnd::f32 alignment = std::max(0.0f, math::dot(-light_dir, normal));
	//    // math::vec3 diffuse_color = tex->sample(vert.tex_coord.x, vert.tex_coord.y);
	//    math::vec3 diffuse = light_color * alignment * diffuse_color;

	//    // specular lighting
	//    math::vec3 specular;
	//    if (alignment > 0.0f) 
	//    {
	//        math::vec3 view_dir = math::normalize(cam_pos - vert.world_position);
	//        math::vec3 reflected = math::reflect(light_dir, normal);
	//        rnd::f32 phong_val = std::max(0.0f, math::dot(view_dir, reflected));
	//        phong_val = std::pow(phong_val, 32.f);
	//        specular = math::vec3(phong_val) * spec_color * 2.0f;
	//    }

	//    math::vec3 lighting = ambient + diffuse + specular;
	//    math::vec3 color = base_color * lighting;

	//    return rnd::to_color(color);
	//}

	void bind_camera_position(const math::vec3& cam_pos)
	{
		this->cam_pos = cam_pos;
	}

	void bind_diffuse_texture(gfx::texture* tex)
	{
		this->diffuse_tex = tex;
	}

	void bind_specular_texture(gfx::texture* tex)
	{
		this->spec_tex = tex;
	}


	void add_directional_light(directional_light d_light)
	{
		directional_lights.push_back(d_light);
	}

	void add_point_light(point_light p_light)
	{
		point_lights.push_back(p_light);
	}

	void bind_time(rnd::f32 time) {
		this->time = time;
	}

private:
	std::vector<directional_light> directional_lights;
	std::vector<point_light> point_lights;

	gfx::texture* diffuse_tex = nullptr;
	gfx::texture* spec_tex = nullptr;
	math::vec3 cam_pos = {0.f, 0.f, 0.f};
	rnd::f32 time = 0.0f;
};

struct triangle_to_render 
{
	rnd::f32 B01;
	rnd::f32 B12;
	rnd::f32 B20;

	rnd::f32 A01;
	rnd::f32 A12;
	rnd::f32 A20;

	rnd::i32 xmin, xmax;
	rnd::i32 ymin, ymax;

	rnd::f32 rcp_area;

	rnd::f32 fy1, fy2, fy3;

	vertex_output v0, v1, v2;
};

template <typename VS, typename FS>
class renderer
{
public:
	renderer(rnd::framebuffer& fb) : fb(fb) {}

	void draw_wireframe(const math::mat4& mvp, rnd::color color, rnd::f32 time)
	{
		for (rnd::i32 i = 0; i < mesh.positions.size(); ++i)
		{
			math::vec3 pos = mesh.positions[i];

			static rnd::f32 amplitude = 0.1f;
			static rnd::f32 freq_scroll = 5.0f;
			static rnd::f32 freq_wave = 10.0f;
			//pos.y += amplitude * sin(time * freqScroll + pos.x * freqWave);
			pos.z += amplitude * std::sin(pos.x * freq_wave + time* freq_scroll);

			math::vec4 clip_pos = mvp * math::vec4(pos, 1.0f);
			clip_pos = viewport.transform(perspective_divide(clip_pos));
			mesh.positions[i] = clip_pos;
		}

		gfx::draw_mesh_wireframe(mesh, color, fb);
	}

	// main function for pipeline processing
	void draw()
	{
		gfx::mesh curr_mesh = mesh; // copy of the mesh
		rnd::sz num_triangles = curr_mesh.indices.size() / 3;

		for (rnd::sz i = 0; i < num_triangles; ++i)
		{
			const vertex_input verts[3] = {
				vertex_input {
					.position = curr_mesh.positions[curr_mesh.indices[i * 3 + 0]],
					.world_position = curr_mesh.positions[curr_mesh.indices[i * 3 + 0]],
					.tex_coord = curr_mesh.tex_coords[curr_mesh.indices[i * 3 + 0]],
					.normal = curr_mesh.normals[curr_mesh.indices[i * 3 + 0]],
					.color = curr_mesh.colors[curr_mesh.indices[i * 3 + 0]],
				},
				vertex_input {
					.position = curr_mesh.positions[curr_mesh.indices[i * 3 + 1]],
					.world_position = curr_mesh.positions[curr_mesh.indices[i * 3 + 1]],
					.tex_coord = curr_mesh.tex_coords[curr_mesh.indices[i * 3 + 1]],
					.normal = curr_mesh.normals[curr_mesh.indices[i * 3 + 1]],
					.color = curr_mesh.colors[curr_mesh.indices[i * 3 + 1]],
				},
				vertex_input {
					.position = curr_mesh.positions[curr_mesh.indices[i * 3 + 2]],
					.world_position = curr_mesh.positions[curr_mesh.indices[i * 3 + 2]],
					.tex_coord = curr_mesh.tex_coords[curr_mesh.indices[i * 3 + 2]],
					.normal = curr_mesh.normals[curr_mesh.indices[i * 3 + 2]],
					.color = curr_mesh.colors[curr_mesh.indices[i * 3 + 2]],
				}
			};

			// vertex shader stage
			vertex_output vsout[3];
			for (rnd::i32 i = 0; i < 3; ++i)
			{
				vsout[i] = (*vertex_shader)(verts[i]);
			}

			// clipping stage
			vertex_output clipped[12];
			rnd::sz cnt = perform_clipping(vsout, clipped);

			rnd::sz n_clipped_triangles = cnt / 3;
			for (rnd::sz clipped_tri = 0; clipped_tri < n_clipped_triangles; ++clipped_tri)
			{
				vertex_output& v0 = clipped[clipped_tri * 3 + 0];
				vertex_output& v1 = clipped[clipped_tri * 3 + 1];
				vertex_output& v2 = clipped[clipped_tri * 3 + 2];

				vertex_output verts_clipped[3] = { v0, v1, v2 };
				
				// projection
				for (rnd::sz i = 0; i < 3; ++i)
				{
					verts_clipped[i].position = viewport.transform(perspective_divide(verts_clipped[i].position));

					verts_clipped[i].normal = verts_clipped[i].normal * verts_clipped[i].position.w;
					verts_clipped[i].tex_coord = verts_clipped[i].tex_coord * verts_clipped[i].position.w;
					verts_clipped[i].world_position = verts_clipped[i].world_position * verts_clipped[i].position.w;
				}

				rnd::f32 area = math::det_2d(
					verts_clipped[1].position - verts_clipped[0].position,
					verts_clipped[2].position - verts_clipped[0].position
				);

				// backface culling
				const rnd::b8 ccw = area < 0.f;
				if (!ccw)
					continue;
				std::swap(verts_clipped[1], verts_clipped[2]);
				area = -area;

				triangle_to_render tri = setup_triangle(verts_clipped, area);

				draw_triangle_basic(tri.v0, tri.v1, tri.v2, area);
				//draw_triangle_raw(tri);
				//draw_triangle_incremental(tri);
			}
		}
	}

	void draw_triangle_raw(const triangle_to_render& tri)
	{
		for (rnd::i32 y = tri.ymin; y <= tri.ymax; y += 2)
		{
			for (rnd::i32 x = tri.xmin; x <= tri.xmax; x += 2)
			{
				rnd::f32 det01p[2][2];
				rnd::f32 det12p[2][2];
				rnd::f32 det20p[2][2];

				rnd::f32 alpha[2][2];
				rnd::f32 beta [2][2];
				rnd::f32 gamma[2][2];

				math::vec2 tex_coord[2][2];

				for (rnd::i32 dy = 0; dy < 2; ++dy)
				{
					for (rnd::i32 dx = 0; dx < 2; ++dx)
					{
						math::vec4 p{ x + dx + 0.5f, y + dy + 0.5f, 0.0f, 0.0f };
						det01p[dy][dx] = math::det_2d(tri.v1.position - tri.v0.position, p - tri.v0.position);
						det12p[dy][dx] = math::det_2d(tri.v2.position - tri.v1.position, p - tri.v1.position);
						det20p[dy][dx] = math::det_2d(tri.v0.position - tri.v2.position, p - tri.v2.position);

						alpha[dy][dx] = det12p[dy][dx] * tri.rcp_area;
						beta [dy][dx] = det20p[dy][dx] * tri.rcp_area;
						gamma[dy][dx] = det01p[dy][dx] * tri.rcp_area;

						rnd::f32 oneOverZ = alpha[dy][dx] * tri.v0.position.w + beta[dy][dx] * tri.v1.position.w + gamma[dy][dx] * tri.v2.position.w;
						rnd::f32 z = 1.0f / oneOverZ;

						tex_coord[dy][dx] = (alpha[dy][dx] * tri.v0.tex_coord + beta[dy][dx] * tri.v1.tex_coord + gamma[dy][dx] * tri.v2.tex_coord) * z;
					}
				}

				for (rnd::i32 dy = 0; dy < 2; ++dy)
				{
					for (rnd::i32 dx = 0; dx < 2; ++dx)
					{
						// if the cooridnates are beyond the triangle's bounding box, then CONTINUE
						if (x + dx > tri.xmax)
							continue;
						if (y + dy > tri.ymax)
							continue;

						// If the point is outside of the triangle, then CONTINUE.
						if (det01p[dy][dx] < 0.f || det12p[dy][dx] < 0.f || det20p[dy][dx] < 0.f)
							continue;

						rnd::f32 oneOverZ = alpha[dy][dx] * tri.v0.position.w + beta[dy][dx] * tri.v1.position.w + gamma[dy][dx] * tri.v2.position.w;
						rnd::f32 z = 1.0f / oneOverZ;

						if (z >= fb.get_depth(x+dx, y+dy))
							continue;
						fb.set_depth(x+dx, y+dy, z);

						math::vec2 texture_scale = { (rnd::f32) texture->get_width(), (rnd::f32) texture->get_height() };

						math::vec2 tc_dx = texture_scale * (tex_coord[dy][1] - tex_coord[dy][0]);
						math::vec2 tc_dy = texture_scale * (tex_coord[1][dx] - tex_coord[0][dx]);

						rnd::f32 t1 = tc_dx.x * tc_dx.x + tc_dx.y * tc_dx.y;
						rnd::f32 t2 = tc_dy.x * tc_dy.x + tc_dy.y * tc_dy.y;
						int mipLevel = std::ceil( std::log2(std::max(t1, t2)) * 0.5f );

						if (mipLevel < 0)
							mipLevel = 0;                       

						if (mipLevel > 10)
							mipLevel = 10;
						
						rnd::color test = rnd::white;
						int mipIndex = (int)std::floor((mipLevel));

						switch (mipIndex)
						{
							case 0: test = rnd::red; break;
							case 1: test = rnd::green; break;
							case 2: test = rnd::blue; break;
							case 3: test = rnd::magenta; break;
							case 4: test = rnd::yellow; break;
							case 5: test = rnd::cyan; break;
							default: test = rnd::black; break;
						}


						texture->set_mip_level(mipIndex);
						math::vec4 sample = texture->sample(tex_coord[dy][dx].x, tex_coord[dy][dx].y);
						fb.put_pixel(x + dx, y + dy, rnd::to_color(sample));
						texture->set_mip_level(0);

						//fb.put_pixel(x + dx, y + dy, test);
					}
				}

			}
		}

		/*for (rnd::i32 y = tri.ymin; y <= tri.ymax; y += 2)
		{
			for (rnd::i32 x = tri.xmin; x <= tri.xmax; x += 2)
			{
				math::vec4 p{ x + 0.5f, y + 0.5f, 0.0f, 0.0f };

				rnd::f32 det01p = math::det_2d(tri.v1.position - tri.v0.position, p - tri.v0.position);
				rnd::f32 det12p = math::det_2d(tri.v2.position - tri.v1.position, p - tri.v1.position);
				rnd::f32 det20p = math::det_2d(tri.v0.position - tri.v2.position, p - tri.v2.position);

				if (det01p < 0.f || det12p < 0.f || det20p < 0.f)
					continue;

				rnd::f32 alpha = det12p * tri.rcp_area;
				rnd::f32 beta  = det20p * tri.rcp_area;
				rnd::f32 gamma = det01p * tri.rcp_area;

				rnd::f32 oneOverZ = alpha * tri.v0.position.w + beta * tri.v1.position.w + gamma * tri.v2.position.w;
				rnd::f32 z = 1.0f / oneOverZ;

				if (z >= fb.get_depth(x, y))
					continue;

				fb.set_depth(x, y, z);

				math::vec2 tex_coord = (alpha * tri.v0.tex_coord + beta * tri.v1.tex_coord + gamma * tri.v2.tex_coord) * z;
				tex_coord.x = std::clamp(tex_coord.x, 0.0f, 1.0f);
				tex_coord.y = std::clamp(tex_coord.y, 0.0f, 1.0f);

				vertex_output interpolated = {
					.position = (alpha * tri.v0.position + beta * tri.v1.position + gamma * tri.v2.position) * z,
					.world_position = (alpha * tri.v0.world_position + beta * tri.v1.world_position + gamma * tri.v2.world_position) * z,
					.tex_coord = tex_coord,
					.normal = (alpha * tri.v0.normal + beta * tri.v1.normal + gamma * tri.v2.normal) * z,
					.color = (alpha * tri.v0.color + beta * tri.v1.color + gamma * tri.v2.color) * z
				};

				rnd::color pixel_color = (*fragment_shader)(interpolated);
				fb.put_pixel(x, y, pixel_color);
			}
		}*/

	}

	void draw_triangle_incremental(triangle_to_render& tri)
	{
		for (rnd::i32 y = tri.ymin; y <= tri.ymax; ++y)
		{
			rnd::f32 fx1 = tri.fy1;
			rnd::f32 fx2 = tri.fy2;
			rnd::f32 fx3 = tri.fy3;

			for (rnd::i32 x = tri.xmin; x <= tri.xmax; ++x)
			{
				if (fx1 >= 0 && fx2 >= 0 && fx3 >= 0)
				{
					rnd::f32 alpha = fx2 * tri.rcp_area;
					rnd::f32 beta = fx3 * tri.rcp_area;
					rnd::f32 gamma = fx1 * tri.rcp_area;

					rnd::f32 oneOverZ = alpha * tri.v0.position.w + beta * tri.v1.position.w + gamma * tri.v2.position.w;
					rnd::f32 z = 1.0f / oneOverZ;

					if (z >= fb.get_depth(x, y))
						continue;

					fb.set_depth(x, y, z);

					math::vec2 tex_coord = (alpha * tri.v0.tex_coord + beta * tri.v1.tex_coord + gamma * tri.v2.tex_coord) * z;
					tex_coord.x = std::clamp(tex_coord.x, 0.0f, 1.0f);
					tex_coord.y = std::clamp(tex_coord.y, 0.0f, 1.0f);

					vertex_output interpolated = {
						.position = (alpha * tri.v0.position + beta * tri.v1.position + gamma * tri.v2.position) * z,
						.world_position = (alpha * tri.v0.world_position + beta * tri.v1.world_position + gamma * tri.v2.world_position) * z,
						.tex_coord = tex_coord,
						.normal = (alpha * tri.v0.normal + beta * tri.v1.normal + gamma * tri.v2.normal) * z,
						.color = (alpha * tri.v0.color + beta * tri.v1.color + gamma * tri.v2.color) * z
					};

					rnd::color pixel_color = (*fragment_shader)(interpolated);
					fb.put_pixel(x, y, pixel_color);
				}
				fx1 += tri.A01;
				fx2 += tri.A12;
				fx3 += tri.A20;
			}
			tri.fy1 += tri.B01;
			tri.fy2 += tri.B12;
			tri.fy3 += tri.B20;
		}
	}

	void draw_triangle_basic(vertex_output& v0, vertex_output& v1, vertex_output& v2, rnd::f32 area)
	{
		rnd::f32 rcp_area = 1.f / area;

		rnd::i32 xmin = (rnd::i32)util::min3(v0.position.x, v1.position.x, v2.position.x);
		rnd::i32 xmax = (rnd::i32)util::max3(v0.position.x, v1.position.x, v2.position.x);
		rnd::i32 ymin = (rnd::i32)util::min3(v0.position.y, v1.position.y, v2.position.y);
		rnd::i32 ymax = (rnd::i32)util::max3(v0.position.y, v1.position.y, v2.position.y);

		// Clamp to viewport bounds.
		xmin = std::clamp(xmin, viewport.xmin, viewport.xmax - 1);
		xmax = std::clamp(xmax, viewport.xmin, viewport.xmax - 1);
		ymin = std::clamp(ymin, viewport.ymin, viewport.ymax - 1);
		ymax = std::clamp(ymax, viewport.ymin, viewport.ymax - 1);

		for (rnd::i32 y = ymin; y <= ymax; ++y)
		{
			for (rnd::i32 x = xmin; x <= xmax; ++x)
			{
				math::vec4 p{ x + 0.5f, y + 0.5f, 0.f, 0.f };

				float det01p = math::det_2d(v1.position - v0.position, p - v0.position);
				float det12p = math::det_2d(v2.position - v1.position, p - v1.position);
				float det20p = math::det_2d(v0.position - v2.position, p - v2.position);

				if (det01p < 0.f || det12p < 0.f || det20p < 0.f)
					continue;

				float alpha = det12p * rcp_area;
				float beta = det20p * rcp_area;
				float gamma = det01p * rcp_area;

				math::vec3 color = alpha * v0.color + beta * v1.color + gamma * v2.color;
				//rnd::color color = fragment_shader(vertex_output{});

				fb.put_pixel((int)x, (int)y, rnd::to_color(color));
			}
		}
	}

	// takes a triangle and outputs up to 4 triangles
	rnd::sz perform_clipping(vertex_output triangle_in[3], vertex_output triangles_out[12])
	{
		// clip against Z > -W => Z + W > 0
		math::vec4 plane = { 0.0f, 0.0f, 1.0f, 1.0f };

		rnd::f32 val0 = math::dot(triangle_in[0].position, plane);
		rnd::f32 val1 = math::dot(triangle_in[1].position, plane);
		rnd::f32 val2 = math::dot(triangle_in[2].position, plane);

		// true -> inside, false -> outside
		rnd::b8 t0 = val0 > 0.f;
		rnd::b8 t1 = val1 > 0.f;
		rnd::b8 t2 = val2 > 0.f;

		rnd::u8 pack = 0;
		pack |= t0 ? (1 << 2) : 0;
		pack |= t1 ? (1 << 1) : 0;
		pack |= t2 ? (1 << 0) : 0;

		rnd::sz n_triangles = 0;

		switch (pack)
		{
		case 0b000:
			// all outside
			n_triangles = 0;
			break;
		case 0b001:
		{
			// v0 inside, v1 and v2 outside
			//rnd::f32 i0 = val0 / (val0 - val1);
			//rnd::f32 i1 = val0 / (val0 - val2);

			//triangles_out[0] = triangle_in[0];
			//triangles_out[1].position = (1.f - i0) * triangle_in[0].position * i0;
			//triangles_out[1].world_position = (1.f - i0) * triangle_in[0].world_position * i0;
			//triangles_out[1].normal = (1.f - i0) * triangle_in[0].normal * i0;
			//triangles_out[1].tex_coord = (1.f - i0) * triangle_in[0].tex_coord * i0;

			//triangles_out[2].position = (1.f - i1) * triangle_in[0].position * i1;
			//triangles_out[2].world_position = (1.f - i1) * triangle_in[0].world_position * i1;
			//triangles_out[2].normal = (1.f - i1) * triangle_in[0].normal * i1;
			//triangles_out[2].tex_coord = (1.f - i1) * triangle_in[0].tex_coord * i1;
			//n_triangles = 3;
		}
		break;
		case 0b011:
		{
			// v0 and v1 inside, v2 outside

			rnd::f32 i0 = val2 / (val2 - val0); // intersection point of v0 and v2
			rnd::f32 i1 = val2 / (val2 - val1); // intersection point of v1 and v2

			vertex_output new_tri_0;
			new_tri_0.position =        (1.f - i0) *       triangle_in[2].position       + i0 *         triangle_in[0].position;
			new_tri_0.world_position =  (1.f - i0) *       triangle_in[2].world_position + i0 *         triangle_in[0].world_position;
			new_tri_0.normal =          (1.f - i0) *       triangle_in[2].normal         + i0 *         triangle_in[0].normal;
			new_tri_0.tex_coord =       (1.f - i0) *       triangle_in[2].tex_coord      + i0 *         triangle_in[0].tex_coord;
			new_tri_0.color =           (1.f - i0) *       triangle_in[2].color          + i0 *         triangle_in[0].color;

			vertex_output new_tri_1;
			new_tri_1.position =        (1.f - i1) *       triangle_in[2].position       + i1 *         triangle_in[1].position;
			new_tri_1.world_position =  (1.f - i1) *       triangle_in[2].world_position + i1 *         triangle_in[1].world_position;
			new_tri_1.normal =          (1.f - i1) *       triangle_in[2].normal         + i1 *         triangle_in[1].normal;
			new_tri_1.tex_coord =       (1.f - i1) *       triangle_in[2].tex_coord      + i1 *         triangle_in[1].tex_coord;
			new_tri_1.color =           (1.f - i1) *       triangle_in[2].color          + i1 *         triangle_in[1].color;

			triangles_out[0] = triangle_in[0];
			triangles_out[1] = triangle_in[1];
			triangles_out[2] = new_tri_0;

			triangles_out[3] = new_tri_0;
			triangles_out[4] = triangle_in[1];
			triangles_out[5] = new_tri_1;
			n_triangles = 6;
		}
		break;
		case 0b111:
			// all inside
			triangles_out[0] = triangle_in[0];
			triangles_out[1] = triangle_in[1];
			triangles_out[2] = triangle_in[2];
			n_triangles = 3;
			break;
		default:
			ASSERT(false, "fail");
			break;
		}

		return n_triangles;
	}

	void bind_vertex_shader(VS* vs)
	{
		vertex_shader = vs;
	}

	void bind_fragment_shader(FS* fs)
	{
		fragment_shader = fs;
	}

	void bind_viewport(const viewport& vp)
	{
		viewport = vp;
	}

	void bind_mesh(gfx::mesh mesh)
	{
		this->mesh = std::move(mesh);
	}

	void bind_texture(gfx::texture* tex)
	{
		this->texture = tex;
	}
private:
	inline triangle_to_render setup_triangle(vertex_output vsout[3], rnd::f32 area)
	{
		triangle_to_render tri{};

		tri.rcp_area = 1.f / area;

		rnd::i32 xmin = (rnd::i32)util::min3(vsout[0].position.x, vsout[1].position.x, vsout[2].position.x);
		rnd::i32 xmax = (rnd::i32)util::max3(vsout[0].position.x, vsout[1].position.x, vsout[2].position.x);

		rnd::i32 ymin = (rnd::i32)util::min3(vsout[0].position.y, vsout[1].position.y, vsout[2].position.y);
		rnd::i32 ymax = (rnd::i32)util::max3(vsout[0].position.y, vsout[1].position.y, vsout[2].position.y);

		// Clamp to viewport bounds.
		xmin = std::clamp(xmin, viewport.xmin, viewport.xmax - 1);
		xmax = std::clamp(xmax, viewport.xmin, viewport.xmax - 1);
		ymin = std::clamp(ymin, viewport.ymin, viewport.ymax - 1);
		ymax = std::clamp(ymax, viewport.ymin, viewport.ymax - 1);

		tri.xmax = xmax;
		tri.xmin = xmin;
		tri.ymin = ymin;
		tri.ymax = ymax;

		tri.A01 = vsout[0].position.y - vsout[1].position.y;
		tri.A12 = vsout[1].position.y - vsout[2].position.y;
		tri.A20 = vsout[2].position.y - vsout[0].position.y;

		tri.B01 = vsout[1].position.x - vsout[0].position.x;
		tri.B12 = vsout[2].position.x - vsout[1].position.x;
		tri.B20 = vsout[0].position.x - vsout[2].position.x;

		// Edge function constants.
		rnd::f32 C1 = -vsout[0].position.y * vsout[1].position.x + vsout[1].position.y * vsout[0].position.x;
		rnd::f32 C2 = -vsout[1].position.y * vsout[2].position.x + vsout[2].position.y * vsout[1].position.x;
		rnd::f32 C3 = -vsout[2].position.y * vsout[0].position.x + vsout[0].position.y * vsout[2].position.x;

		// Correct for fill convention.
		//if (tri.A01 < 0 || (tri.A01 == 0 && tri.B01 > 0)) C1++;
		//if (tri.A12 < 0 || (tri.A12 == 0 && tri.B12 > 0)) C2++;
		//if (tri.A20 < 0 || (tri.A20 == 0 && tri.B20 > 0)) C3++;

		//if (tri.A01 > 0 || (tri.A01 == 0 && tri.B01 < 0)) C1++;
		//if (tri.A12 > 0 || (tri.A12 == 0 && tri.B12 < 0)) C2++;
		//if (tri.A20 > 0 || (tri.A20 == 0 && tri.B20 < 0)) C3++;

		tri.fy1 = tri.B01 * ymin + tri.A01 * xmin + C1;
		tri.fy2 = tri.B12 * ymin + tri.A12 * xmin + C2;
		tri.fy3 = tri.B20 * ymin + tri.A20 * xmin + C3;

		tri.v0 = vsout[0];
		tri.v1 = vsout[1];
		tri.v2 = vsout[2];

		return tri;
	}

	inline static math::vec4 perspective_divide(math::vec4 v)
	{
		v.w = 1.f / v.w;
		v.x *= v.w;
		v.y *= v.w;
		v.z *= v.w;
		return v;
	}
private:
	rnd::framebuffer& fb;

	VS* vertex_shader = nullptr;
	FS* fragment_shader = nullptr;

	struct viewport viewport { 0, 0, 0, 0 };
	gfx::mesh mesh;
	gfx::texture* texture = nullptr;
};