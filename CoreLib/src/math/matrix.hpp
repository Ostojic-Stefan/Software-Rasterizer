#pragma once

#include <initializer_list>
#include <cmath>
#include <algorithm>
#include "vector.hpp"

namespace math
{
    struct mat4 {
        float values[16];

        // Default constructor.
        mat4() = default;

        // Construct from an initializer list.
        mat4(std::initializer_list<float> list) {
            std::copy(list.begin(), list.end(), values);
        }

        // Returns the identity matrix (column-major).
        static mat4 identity() {
            return mat4{
                // Column 0
                1.f, 0.f, 0.f, 0.f,
                // Column 1
                0.f, 1.f, 0.f, 0.f,
                // Column 2
                0.f, 0.f, 1.f, 0.f,
                // Column 3
                0.f, 0.f, 0.f, 1.f
            };
        }

        // Scale matrix (column-major).
        static mat4 scale(const vec3& s) {
            return mat4{
                // Column 0
                s.x, 0.f, 0.f, 0.f,
                // Column 1
                0.f, s.y, 0.f, 0.f,
                // Column 2
                0.f, 0.f, s.z, 0.f,
                // Column 3
                0.f, 0.f, 0.f, 1.f
            };
        }

        static mat4 scale(float s) {
            return scale(vec3{ s, s, s });
        }

        // Translation matrix (column-major).
        static mat4 translate(const vec3& t) {
            return mat4{
                // Column 0
                1.f, 0.f, 0.f, 0.f,
                // Column 1
                0.f, 1.f, 0.f, 0.f,
                // Column 2
                0.f, 0.f, 1.f, 0.f,
                // Column 3 (translation components)
                t.x, t.y, t.z, 1.f
            };
        }

        // Rotation about X axis (column-major).
        static mat4 rotation_x(float angle) {
            float c = std::cos(angle);
            float s = std::sin(angle);
            return mat4{
                // Column 0
                1.f, 0.f, 0.f, 0.f,
                // Column 1
                0.f, c,    s,   0.f,
                // Column 2
                0.f, -s,   c,   0.f,
                // Column 3
                0.f, 0.f, 0.f, 1.f
            };
        }

        // Rotation about Y axis (column-major).
        static mat4 rotation_y(float angle) {
            float c = std::cos(angle);
            float s = std::sin(angle);
            return mat4{
                // Column 0
                c,   0.f, -s,  0.f,
                // Column 1
                0.f, 1.f,  0.f, 0.f,
                // Column 2
                s,   0.f,  c,   0.f,
                // Column 3
                0.f, 0.f, 0.f,  1.f
            };
        }

        // Rotation about Z axis (column-major).
        static mat4 rotation_z(float angle) {
            float c = std::cos(angle);
            float s = std::sin(angle);
            return mat4{
                // Column 0
                c,   s,   0.f, 0.f,
                // Column 1
                -s,  c,   0.f, 0.f,
                // Column 2
                0.f, 0.f, 1.f,  0.f,
                // Column 3
                0.f, 0.f, 0.f,  1.f
            };
        }

        static mat4 perspective(float near, float far, float fovy, float aspect_ratio) {
            mat4 result{};
            float tanHalfFovy = std::tan(fovy / 2.0f);
            
            // Column 0
            result.values[0] = 1.0f / (aspect_ratio * tanHalfFovy);
            result.values[1] = 0.f;
            result.values[2] = 0.f;
            result.values[3] = 0.f;

            // Column 1
            result.values[4] = 0.f;
            result.values[5] = 1.0f / tanHalfFovy;
            result.values[6] = 0.f;
            result.values[7] = 0.f;

            // Column 2
            result.values[8] = 0.f;
            result.values[9] = 0.f;
            result.values[10] = -(far + near) / (far - near);
            result.values[11] = -1.f;

            // Column 3
            result.values[12] = 0.f;
            result.values[13] = 0.f;
            result.values[14] = -(2.f * far * near) / (far - near);
            result.values[15] = 0.f;

            return result;
        }

        // OpenGL-style look_at (column-major).
        static inline mat4 look_at(const vec3& eye, const vec3& center, const vec3& up) {
            vec3 f = normalize(math::vec3{ center.x - eye.x, center.y - eye.y, center.z - eye.z });
            vec3 s = normalize(cross(f, up));
            vec3 u = cross(s, f);

            mat4 result = identity();

            // First column
            result.values[0] = s.x;
            result.values[1] = s.y;
            result.values[2] = s.z;
            result.values[3] = 0.f;

            // Second column
            result.values[4] = u.x;
            result.values[5] = u.y;
            result.values[6] = u.z;
            result.values[7] = 0.f;

            // Third column (negative forward)
            result.values[8] = -f.x;
            result.values[9] = -f.y;
            result.values[10] = -f.z;
            result.values[11] = 0.f;

            // Fourth column (translation)
            result.values[12] = -dot(s, eye);
            result.values[13] = -dot(u, eye);
            result.values[14] = dot(f, eye);
            result.values[15] = 1.f;

            return result;
        }

        static mat4 orthographic(float left, float right, float bottom, float top, float near, float far) {
            mat4 result{};

            // Column 0
            result.values[0] = 2.f / (right - left);
            result.values[1] = 0.f;
            result.values[2] = 0.f;
            result.values[3] = 0.f;

            // Column 1
            result.values[4] = 0.f;
            result.values[5] = 2.f / (top - bottom);
            result.values[6] = 0.f;
            result.values[7] = 0.f;

            // Column 2
            result.values[8] = 0.f;
            result.values[9] = 0.f;
            result.values[10] = -2.f / (far - near);
            result.values[11] = 0.f;

            // Column 3
            result.values[12] = -(right + left) / (right - left);
            result.values[13] = -(top + bottom) / (top - bottom);
            result.values[14] = -(far + near) / (far - near);
            result.values[15] = 1.f;

            return result;
        }
    };

    // Multiply matrix (column-major) by vector.
    static inline vec4 operator*(const mat4& m, const vec4& v) {
        vec4 result;
        result.x = m.values[0] * v.x + m.values[4] * v.y + m.values[8] * v.z + m.values[12] * v.w;
        result.y = m.values[1] * v.x + m.values[5] * v.y + m.values[9] * v.z + m.values[13] * v.w;
        result.z = m.values[2] * v.x + m.values[6] * v.y + m.values[10] * v.z + m.values[14] * v.w;
        result.w = m.values[3] * v.x + m.values[7] * v.y + m.values[11] * v.z + m.values[15] * v.w;
        return result;
    }

    // Multiply two matrices (both in column-major).
    static inline mat4 operator*(const mat4& m1, const mat4& m2) {
        mat4 result{};
        // For each column of the result...
        for (int col = 0; col < 4; ++col) {
            // For each row of the result...
            for (int row = 0; row < 4; ++row) {
                float sum = 0.f;
                // Sum over k.
                for (int k = 0; k < 4; ++k) {
                    // In column-major order: element (row, col) is at index col*4 + row.
                    sum += m1.values[k * 4 + row] * m2.values[col * 4 + k];
                }
                result.values[col * 4 + row] = sum;
            }
        }
        return result;
    }
}