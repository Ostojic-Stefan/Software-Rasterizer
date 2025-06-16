#pragma once

#include <Engine/engine.hpp>
#include "viewport.hpp"
#include "light.hpp"

struct shader_program
{
    struct vertex_input
    {
        math::vec3 position;
        math::vec4 color;
    };

    struct vertex_output
    {
        math::vec4 position;
        math::vec4 color;
    };

    struct vertex_shader
    {
        vertex_output operator()(const vertex_input& vs_in)
        {
            return vertex_output{
                .position = math::mat4::rotation_y(total_time) * math::vec4(vs_in.position, 1.0f),
                .color = vs_in.color
            };
        }

    public:
        rnd::f32 total_time = 0.f;
    };

    struct fragment_shader
    {
        math::vec4 operator()(const vertex_output& vs_out)
        {
            return math::vec4(vs_out.color);
        }
    };

    vertex_shader vs;
    fragment_shader fs;
};

static shader_program::vertex_output operator*(rnd::f32 s, const shader_program::vertex_output& vout)
{
    return {
        .position = vout.position * s,
        .color = vout.color * s
    };
}

static shader_program::vertex_output operator+(const shader_program::vertex_output& vout1, const shader_program::vertex_output& vout2)
{
    return {
        .position = vout1.position + vout2.position,
        .color = vout1.color + vout2.color
    };
}


template <typename ShaderProgram>
class the_renderer
{
public:
    typedef typename ShaderProgram::vertex_input VSIn;
    typedef typename ShaderProgram::vertex_output VSOut;
public:
    the_renderer(rnd::framebuffer& fb)
        : fb(fb) {}

    void bind_vertex_buffer(const std::vector<VSIn>& vb)
    {
        // todo: currently it's copying
        vertex_buffer = vb;
    }

    void bind_index_buffer(const std::vector<rnd::u16>& ib)
    {
        // todo: currently it's copying
        index_buffer = ib;
    }

    void render_indexed()
    {
        float total_time = SDL_GetTicks() / 1000.f;
        program.vs.total_time = total_time;
        rnd::i32 num_triangles = index_buffer.size() / 3.f;
        for (rnd::i32 i = 0; i < num_triangles; ++i)
        {
            const VSIn verts[3] = {
                vertex_buffer[index_buffer[i * 3 + 0]],
                vertex_buffer[index_buffer[i * 3 + 1]],
                vertex_buffer[index_buffer[i * 3 + 2]]
            };

            VSOut vsout[3];
            for (rnd::i32 i = 0; i < 3; ++i)
            {
                vsout[i] = program.vs(verts[i]);

                vsout[i].position = _viewport.transform(perspective_divide(vsout[i].position));
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

            draw_triangle_basic(vsout[0], vsout[1], vsout[2], area);
        }
    }

    void draw_triangle_basic(VSOut& v0, VSOut& v1, VSOut& v2, rnd::f32 area)
    {
        rnd::f32 rcp_area = 1.f / area;

        rnd::i32 xmin = (rnd::i32)util::min3(v0.position.x, v1.position.x, v2.position.x);
        rnd::i32 xmax = (rnd::i32)util::max3(v0.position.x, v1.position.x, v2.position.x);
        rnd::i32 ymin = (rnd::i32)util::min3(v0.position.y, v1.position.y, v2.position.y);
        rnd::i32 ymax = (rnd::i32)util::max3(v0.position.y, v1.position.y, v2.position.y);

        // Clamp to viewport bounds.
        xmin = std::clamp(xmin, _viewport.xmin, _viewport.xmax - 1);
        xmax = std::clamp(xmax, _viewport.xmin, _viewport.xmax - 1);
        ymin = std::clamp(ymin, _viewport.ymin, _viewport.ymax - 1);
        ymax = std::clamp(ymax, _viewport.ymin, _viewport.ymax - 1);

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

                VSOut interpolated = alpha * v0 + beta * v1 + gamma * v2;
                math::vec4 color = program.fs(interpolated);

                fb.put_pixel((int)x, (int)y, rnd::to_color(color));
            }
        }
    }

private:
    inline static math::vec4 perspective_divide(math::vec4 v)
    {
        v.w = 1.f / v.w;
        v.x *= v.w;
        v.y *= v.w;
        v.z *= v.w;
        return v;
    }

public:
    rnd::framebuffer& fb;
    ShaderProgram program;
    // TODO: un-hard-code
    viewport _viewport = { 0, 0, 800, 600 };

    std::vector<rnd::u16> index_buffer;
    std::vector<VSIn> vertex_buffer;
};