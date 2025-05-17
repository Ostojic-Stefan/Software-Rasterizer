#pragma once

#include "vector.hpp"

#include <cmath>

namespace rasterizer
{

	struct matrix4x4f
	{
		float values[16];

		static matrix4x4f identity()
		{
			return matrix4x4f{
				1.f, 0.f, 0.f, 0.f,
				0.f, 1.f, 0.f, 0.f,
				0.f, 0.f, 1.f, 0.f,
				0.f, 0.f, 0.f, 1.f,
			};
		}

		static matrix4x4f scale(vector3f const & s)
		{
			return matrix4x4f{
				s.x, 0.f, 0.f, 0.f,
				0.f, s.y, 0.f, 0.f,
				0.f, 0.f, s.z, 0.f,
				0.f, 0.f, 0.f, 1.f,
			};
		}

		static matrix4x4f scale(float s)
		{
			return scale(vector3f{s, s, s});
		}

		static matrix4x4f translate(vector3f const & s)
		{
			return matrix4x4f{
				1.f, 0.f, 0.f, s.x,
				0.f, 1.f, 0.f, s.y,
				0.f, 0.f, 1.f, s.z,
				0.f, 0.f, 0.f, 1.f,
			};
		}

		static matrix4x4f rotateXY(float angle)
		{
			float cos = std::cos(angle);
			float sin = std::sin(angle);

			return matrix4x4f{
				cos, -sin, 0.f, 0.f,
				sin,  cos, 0.f, 0.f,
				0.f,  0.f, 1.f, 0.f,
				0.f,  0.f, 0.f, 1.f,
			};
		}

		static matrix4x4f rotateYZ(float angle)
		{
			float cos = std::cos(angle);
			float sin = std::sin(angle);

			return matrix4x4f{
				1.f, 0.f,  0.f, 0.f,
				0.f, cos, -sin, 0.f,
				0.f, sin,  cos, 0.f,
				0.f, 0.f,  0.f, 1.f,
			};
		}

		static matrix4x4f rotateZX(float angle)
		{
			float cos = std::cos(angle);
			float sin = std::sin(angle);

			return matrix4x4f{
				 cos, 0.f, sin, 0.f,
				 0.f, 1.f, 0.f, 0.f,
				-sin, 0.f, cos, 0.f,
				 0.f, 0.f, 0.f, 1.f,
			};
		}

		static matrix4x4f perspective(float near, float far, float fovY, float aspect_ratio)
		{
			float top = near * std::tan(fovY / 2.f);
			float right = top * aspect_ratio;

			return matrix4x4f
			{
				near / right, 0.f, 0.f, 0.f,
				0.f, near / top, 0.f, 0.f,
				0.f, 0.f, -(far + near) / (far - near), - 2.f * far * near / (far - near),
				0.f, 0.f, -1.f, 0.f,
			};
		}
	};

	inline vector4f operator * (matrix4x4f const & m, vector4f const & v)
	{
		vector4f result{0.f, 0.f, 0.f, 0.f};

		result.x = m.values[ 0] * v.x + m.values[ 1] * v.y + m.values[ 2] * v.z + m.values[ 3] * v.w;
		result.y = m.values[ 4] * v.x + m.values[ 5] * v.y + m.values[ 6] * v.z + m.values[ 7] * v.w;
		result.z = m.values[ 8] * v.x + m.values[ 9] * v.y + m.values[10] * v.z + m.values[11] * v.w;
		result.w = m.values[12] * v.x + m.values[13] * v.y + m.values[14] * v.z + m.values[15] * v.w;

		return result;
	}

	inline matrix4x4f operator * (matrix4x4f const & m1, matrix4x4f const & m2)
	{
		matrix4x4f result
		{
			0.f, 0.f, 0.f, 0.f,
			0.f, 0.f, 0.f, 0.f,
			0.f, 0.f, 0.f, 0.f,
			0.f, 0.f, 0.f, 0.f,
		};

		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				for (int k = 0; k < 4; ++k)
					result.values[4 * i + j] += m1.values[4 * i + k] * m2.values[4 * k + j];

		return result;
	}

	inline static matrix4x4f lookAt(vector3f const& eye, vector3f const& center, vector3f const& up)
	{
		// Compute forward vector (f), which points from the eye towards the center.
		vector3f f = normalized(center - eye);
		// Compute the right vector (s) as the normalized cross product of f and up.
		vector3f s = normalized(cross(f, up));
		// Recompute the orthogonal up vector (u) as the cross product of s and f.
		vector3f u = cross(s, f);

		matrix4x4f result = matrix4x4f::identity();

		// Set the first row to the right vector and its translation component.
		result.values[0] = s.x;
		result.values[1] = s.y;
		result.values[2] = s.z;
		result.values[3] = -dot(s, eye);

		// Set the second row to the up vector and its translation component.
		result.values[4] = u.x;
		result.values[5] = u.y;
		result.values[6] = u.z;
		result.values[7] = -dot(u, eye);

		// Set the third row to the negative forward vector and its translation.
		result.values[8] = -f.x;
		result.values[9] = -f.y;
		result.values[10] = -f.z;
		result.values[11] = dot(f, eye);

		// The bottom row remains as [0, 0, 0, 1] from identity().
		return result;
	}


}
