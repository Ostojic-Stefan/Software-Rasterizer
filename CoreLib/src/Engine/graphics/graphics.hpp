#pragma once

#include "frame_buffer.hpp"
#include "types.hpp"
#include "math/point.hpp"
#include "mesh.hpp"

namespace gfx
{
    struct triangle
    {
        math::vec3 v0;
        math::vec3 v1;
        math::vec3 v2;
    };

	static inline void draw_line(const math::vec2i& start, const math::vec2i& end, rnd::color color, rnd::framebuffer& fb)
	{
        math::vec2i diff = math::vec2i{
            (end.x - start.x),
            (end.y - start.y)
        };

        rnd::i32 it_side = (abs(diff.x) >= abs(diff.y)) ? abs(diff.x) : abs(diff.y);
        math::vec2 step = (math::vec2) diff / (float) it_side;
        math::vec2 it = start;

        for (rnd::i32 i = 0; i <= it_side; i++) 
        {
            fb.put_pixel((rnd::i32)it.x, (rnd::i32)it.y, color);
            it += step;
        }
	}

    template <typename T>
    static inline void draw_point(const math::point2d<T>& pt, rnd::color color, rnd::framebuffer& fb, const math::point2d<T>& padding = { 2, 2 })
    {
        static math::point2d<T> zero = {0, 0};

        math::point2d<T> start = math::clamp(pt - padding, zero, (math::point2d<T>)(fb.get_dimensions()-1));
        math::point2d<T> end   = math::clamp(pt + padding, zero, (math::point2d<T>)(fb.get_dimensions()-1));

        for (rnd::i32 y = start.y; y < end.y; ++y)
        {
            for (rnd::i32 x = start.x; x < end.x; ++x)
            {
                fb.put_pixel(x, y, color);
            }
        }
    }

    static inline void draw_triangle_outline(const triangle& t, const rnd::color& line_color, rnd::framebuffer& fb)
    {
        const auto& [v0, v1, v2] = t;
        draw_line(v0, v1, line_color, fb);
        draw_line(v1, v2, line_color, fb);
        draw_line(v2, v0, line_color, fb);
    }

    static inline void draw_mesh_wireframe(const gfx::mesh& mesh, const rnd::color& line_color, rnd::framebuffer& fb)
    {
        ASSERT(mesh.indices.size() % 3 == 0, "number of indices should be a multiple of 3");

        rnd::sz triangle_count = mesh.indices.size() / 3;
        for (rnd::u32 i = 0; i < triangle_count; i++)
        {
            const math::vec3& v0 = mesh.positions[mesh.indices[i * 3 + 0]];
            const math::vec3& v1 = mesh.positions[mesh.indices[i * 3 + 1]];
            const math::vec3& v2 = mesh.positions[mesh.indices[i * 3 + 2]];

            gfx::draw_triangle_outline({ v0, v1, v2 }, line_color, fb);
        }
    }
}