#pragma once

#include <initializer_list>
#include <cmath>
#include <algorithm>
#include "vector.hpp"

namespace math
{
    struct mat4
    {
        rnd::f32 values[16];

        mat4() = default;

        mat4(std::initializer_list<rnd::f32> list)
        {
            std::copy(list.begin(), list.end(), values);
        }

        static mat4 identity()
        {
            return mat4{
                1.f, 0.f, 0.f, 0.f,
                0.f, 1.f, 0.f, 0.f,
                0.f, 0.f, 1.f, 0.f,
                0.f, 0.f, 0.f, 1.f
            };
        }

        static mat4 scale(const vec3& s)
        {
            return mat4{
                s.x, 0.f, 0.f, 0.f,
                0.f, s.y, 0.f, 0.f,
                0.f, 0.f, s.z, 0.f,
                0.f, 0.f, 0.f, 1.f
            };
        }

        static mat4 scale(rnd::f32 s)
        {
            return scale(vec3{ s, s, s });
        }

        static mat4 translate(const vec3& t)
        {
            return mat4{
                1.f, 0.f, 0.f, 0.f,
                0.f, 1.f, 0.f, 0.f,
                0.f, 0.f, 1.f, 0.f,
                t.x, t.y, t.z, 1.f
            };
        }

        static mat4 rotation_x(rnd::f32 angle)
        {
            rnd::f32 c = std::cos(angle);
            rnd::f32 s = std::sin(angle);
            return mat4{
                1.f, 0.f, 0.f, 0.f,
                0.f,   c,   s, 0.f,
                0.f,  -s,   c, 0.f,
                0.f, 0.f, 0.f, 1.f
            };
        }

        static mat4 rotation_y(rnd::f32 angle)
        {
            rnd::f32 c = std::cos(angle);
            rnd::f32 s = std::sin(angle);
            return mat4{
                  c, 0.f, -s, 0.f,
                0.f, 1.f, 0.f, 0.f,
                  s, 0.f,  c, 0.f,
                0.f, 0.f, 0.f, 1.f
            };
        }

        static mat4 rotation_z(rnd::f32 angle)
        {
            rnd::f32 c = std::cos(angle);
            rnd::f32 s = std::sin(angle);
            return mat4{
                  c,    s, 0.f, 0.f,
                 -s,    c, 0.f, 0.f,
                0.f,  0.f, 1.f, 0.f,
                0.f,  0.f, 0.f, 1.f
            };
        }

        static mat4 perspective(rnd::f32 near, rnd::f32 far, rnd::f32 fovy, rnd::f32 aspect_ratio)
        {
            mat4 result{};  // zero-initialized

            float tanHalfFovy = std::tan(fovy / 2.0f);

            // [  f/aspect    0         0               0   ]
            // [    0         f         0               0   ]
            // [    0         0    (far+near)/(near-far) -1  ]
            // [    0         0  (2*far*near)/(near-far)  0  ]
            //
            // where f = 1 / tan(fovy/2).  This matches glm::perspectiveRH_OpenGL.
            //
            rnd::f32 f = 1.0f / tanHalfFovy;
            result.values[0] = f / aspect_ratio; // col0,row0
            result.values[5] = f;                 // col1,row1

            result.values[10] = (far + near) / (near - far);      // col2,row2
            result.values[11] = -1.0f;                             // col2,row3

            result.values[14] = (2.0f * far * near) / (near - far); // col3,row2

            return result;
        }

        static inline mat4 look_at(const vec3& eye, const vec3& center, const vec3& up)
        {
            // Compute camera basis vectors (right-handed, world-to-view)
            vec3 f = normalize(center - eye);         // “forward”
            vec3 s = normalize(cross(f, up));         // “side” (right)
            vec3 u = cross(s, f);                     // “up” in camera space

            // Build the view matrix exactly as glm::lookAtRH does:
            //
            //  [  s.x    u.x   -f.x    0 ]
            //  [  s.y    u.y   -f.y    0 ]
            //  [  s.z    u.z   -f.z    0 ]
            //  [ -dot(s,eye)  -dot(u,eye)   dot(f,eye)   1 ]
            //
            // In column-major memory (values[col*4 + row]):
            mat4 result = identity();

            // First row = { s.x,  s.y,  s.z,  0 }
            result.values[0] = s.x;  // col0,row0
            result.values[4] = s.y;  // col1,row0
            result.values[8] = s.z;  // col2,row0

            // Second row = { u.x,  u.y,  u.z,  0 }
            result.values[1] = u.x;  // col0,row1
            result.values[5] = u.y;  // col1,row1
            result.values[9] = u.z;  // col2,row1

            // Third row = { -f.x,  -f.y,  -f.z,  0 }
            result.values[2] = -f.x;  // col0,row2
            result.values[6] = -f.y;  // col1,row2
            result.values[10] = -f.z; // col2,row2

            // Fourth row (translation) = { -dot(s,eye),  -dot(u,eye),  dot(f,eye),  1 }
            result.values[12] = -dot(s, eye);  // col0,row3
            result.values[13] = -dot(u, eye);  // col1,row3
            result.values[14] = dot(f, eye);  // col2,row3
            result.values[15] = 1.0f;         // col3,row3

            return result;
        }

        static mat4 orthographic(rnd::f32 left, rnd::f32 right, rnd::f32 bottom, rnd::f32 top, rnd::f32 near, rnd::f32 far)
        {
            mat4 result{}; // zero-initialize

            result.values[0] = 2.f / (right - left);
            result.values[5] = 2.f / (top - bottom);
            result.values[10] = -2.f / (far - near);

            result.values[12] = -(right + left) / (right - left);
            result.values[13] = -(top + bottom) / (top - bottom);
            result.values[14] = -(far + near) / (far - near);
            result.values[15] = 1.f;

            return result;
        }
    };

    static inline vec4 operator*(const mat4& m, const vec4& v)
    {
        vec4 result;
        result.x = m.values[0] * v.x + m.values[4] * v.y + m.values[8] * v.z + m.values[12] * v.w;
        result.y = m.values[1] * v.x + m.values[5] * v.y + m.values[9] * v.z + m.values[13] * v.w;
        result.z = m.values[2] * v.x + m.values[6] * v.y + m.values[10] * v.z + m.values[14] * v.w;
        result.w = m.values[3] * v.x + m.values[7] * v.y + m.values[11] * v.z + m.values[15] * v.w;
        return result;
    }

    static inline mat4 operator*(const mat4& A, const mat4& B)
    {
        mat4 R{};
        for (rnd::i32 col = 0; col < 4; ++col)
        {
            for (rnd::i32 row = 0; row < 4; ++row)
            {
                rnd::f32 sum = 0.f;
                for (rnd::i32 k = 0; k < 4; ++k)
                {
                    // A(row,k) is at A.values[k*4 + row]
                    // B(k,col) is at B.values[col*4 + k]
                    sum += A.values[k * 4 + row] * B.values[col * 4 + k];
                }
                R.values[col * 4 + row] = sum;
            }
        }
        return R;
    }
}