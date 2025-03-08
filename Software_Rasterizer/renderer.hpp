#pragma once

#include <Engine/engine.hpp>
#include "viewport.hpp"

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

static inline const math::vec2 MSAA_SAMPLE_POINTS[4] = {
    { 0.25f, 0.25f }, { 0.75f, 0.25f },
    { 0.25f, 0.75f }, { 0.75f, 0.75f }
};
static inline constexpr rnd::i32 MSAA_SAMPLES = 4;

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

template <typename VertexShader, typename FragmentShader>
class renderer
{
public:
    renderer(rnd::framebuffer& fb) : fb(fb) {}

    // main function for pipeline processing
    void draw()
    {
        gfx::mesh curr_mesh = mesh; // copy of the mesh
        rnd::sz num_triangles = curr_mesh.indices.size() / 3;

        for (rnd::sz i = 0; i < num_triangles; ++i)
        {
            const vertex_input verts[3] = {
                vertex_input  {
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

            vertex_output vsout[3];
            for (rnd::i32 i = 0; i < 3; ++i)
            {
                vsout[i] = vertex_shader(verts[i]);
                vsout[i].position = viewport.transform(perspective_divide(vsout[i].position));
            }

            rnd::f32 area = math::det_2d(
                vsout[1].position - vsout[0].position,
                vsout[2].position - vsout[0].position
            );

            // backface culling
            const rnd::b8 ccw = area < 0.f;
            if (!ccw)
                continue;
            std::swap(vsout[1], vsout[2]);
            area = -area;

            triangle_to_render tri = setup_triangle(vsout, area);

            draw_triangle_incremental(tri);
        }
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
                if (fx1 > 0 && fx2 > 0 && fx3 > 0)
                {
                    //  calculate the perspective corrected interpolants
                    rnd::f32 alpha = fx2 * tri.rcp_area * tri.v0.position.w;
                    rnd::f32 beta =  fx3 * tri.rcp_area * tri.v1.position.w;
                    rnd::f32 gamma = fx1 * tri.rcp_area * tri.v2.position.w;
                    rnd::f32 sum = alpha + beta + gamma;
                    alpha /= sum;
                    beta  /= sum;
                    gamma /= sum;

                    math::vec4 ndc_pos = alpha * tri.v0.position + beta * tri.v1.position + gamma * tri.v2.position;
                    std::uint32_t depth = (0.5f + 0.5 * ndc_pos.z) * MAX_DEPTH;
                    if (!depth_passed(depth, fb.get_depth(x, y)))
                        continue;
                    fb.set_depth(x, y, depth);

                    math::vec2 tex_coord = alpha * tri.v0.tex_coord + beta * tri.v1.tex_coord + gamma * tri.v2.tex_coord;
                    tex_coord.x = std::clamp(tex_coord.x, 0.0f, 1.0f);
                    tex_coord.y = std::clamp(tex_coord.y, 0.0f, 1.0f);

                    vertex_output interpolated = {
                        .position = alpha * tri.v0.position + beta * tri.v1.position + gamma * tri.v2.position,
                        .world_position = alpha * tri.v0.world_position + beta * tri.v1.world_position + gamma * tri.v2.world_position,
                        .tex_coord = tex_coord,
                        .normal = alpha * tri.v0.normal + beta * tri.v1.normal + gamma * tri.v2.normal,
                        .color = alpha * tri.v0.color + beta * tri.v1.color + gamma * tri.v2.color
                    };

                    rnd::color pixel_color = fragment_shader(interpolated);

                    fb.put_pixel(x, y, pixel_color);
                }
                fx1 -= tri.A01;
                fx2 -= tri.A12;
                fx3 -= tri.A20;
            }
            tri.fy1 += tri.B01;
            tri.fy2 += tri.B12;
            tri.fy3 += tri.B20;
        }
    }

    rnd::b8 depth_passed(rnd::u32 depth_value, rnd::u32 ref) const
    {
        return depth_value < ref;
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

                fb.put_pixel((int)x, (int)y, rnd::to_color(color));
            }
        }
    }

    void bind_vertex_shader(VertexShader vs)
    {
        vertex_shader = vs;
    }

    void bind_fragment_shader(FragmentShader fs)
    {
        fragment_shader = fs;
    }

    void bind_viewport(const viewport& vp)
    {
        viewport = vp;
    }

    void bind_mesh(const gfx::mesh& mesh)
    {
        this->mesh = mesh;
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

        tri.A01 = vsout[1].position.y - vsout[0].position.y;
        tri.A12 = vsout[2].position.y - vsout[1].position.y;
        tri.A20 = vsout[0].position.y - vsout[2].position.y;

        tri.B01 = vsout[1].position.x - vsout[0].position.x;
        tri.B12 = vsout[2].position.x - vsout[1].position.x;
        tri.B20 = vsout[0].position.x - vsout[2].position.x;

        // Edge function constants.
        rnd::f32 C1 = tri.A01 * vsout[0].position.x - tri.B01 * vsout[0].position.y;
        rnd::f32 C2 = tri.A12 * vsout[1].position.x - tri.B12 * vsout[1].position.y;
        rnd::f32 C3 = tri.A20 * vsout[2].position.x - tri.B20 * vsout[2].position.y;

        // Correct for fill convention.
        if (tri.A01 < 0 || (tri.A01 == 0 && tri.B01 > 0)) C1++;
        if (tri.A12 < 0 || (tri.A12 == 0 && tri.B12 > 0)) C2++;
        if (tri.A20 < 0 || (tri.A20 == 0 && tri.B20 > 0)) C3++;

        tri.fy1 = C1 + tri.B01 * ymin - tri.A01 * xmin;
        tri.fy2 = C2 + tri.B12 * ymin - tri.A12 * xmin;
        tri.fy3 = C3 + tri.B20 * ymin - tri.A20 * xmin;

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

    VertexShader vertex_shader;
    FragmentShader fragment_shader;

    struct viewport viewport { 0, 0, 0, 0 };
    gfx::mesh mesh;
};