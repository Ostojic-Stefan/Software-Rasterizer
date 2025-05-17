#pragma once

#include <cmath>

namespace rasterizer
{

	struct vector2f
	{
		float x, y;
	};

	inline vector2f operator * (float s, vector2f const & v)
	{
		return {s * v.x, s * v.y};
	}

	inline vector2f operator - (vector2f const & v1, vector2f const & v2)
	{
		return {v1.x - v2.x, v1.y - v2.y};
	}

	inline vector2f operator + (vector2f const & v1, vector2f const & v2)
	{
		return {v1.x + v2.x, v1.y + v2.y};
	}

	inline vector2f operator * (vector2f const & v1, vector2f const & v2)
	{
		return {v1.x * v2.x, v1.y * v2.y};
	}

	inline float det2D(vector2f const & v0, vector2f const & v1)
	{
		return v0.x * v1.y - v0.y * v1.x;
	}

	struct vector3f
	{
		float x, y, z;
	};

	inline vector3f operator * (float s, vector3f const & v)
	{
		return {s * v.x, s * v.y, s * v.z};
	}

	inline vector3f operator / (vector3f const & v, float s)
	{
		return {v.x / s, v.y / s, v.z / s};
	}

	inline vector3f operator - (vector3f const & v1, vector3f const & v2)
	{
		return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
	}

	inline vector3f operator + (vector3f const & v1, vector3f const & v2)
	{
		return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
	}

	inline vector3f operator * (vector3f const & v1, vector3f const & v2)
	{
		return {v1.x * v2.x, v1.y * v2.y, v1.z * v2.z};
	}

	inline float dot(vector3f const & v1, vector3f const & v2)
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	inline float length(vector3f const & v)
	{
		return std::sqrt(dot(v, v));
	}

	inline vector3f normalized(vector3f const & v)
	{
		return v / length(v);
	}

	inline vector3f cross(vector3f const & v1, vector3f const & v2)
	{
		return {v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.z};
	}

	struct vector4f
	{
		float x, y, z, w;
	};

	inline vector4f operator * (float s, vector4f const & v)
	{
		return {s * v.x, s * v.y, s * v.z, s * v.w};
	}

	inline vector4f operator / (vector4f const & v, float s)
	{
		return {v.x / s, v.y / s, v.z / s, v.w / s};
	}

	inline vector4f operator - (vector4f const & v0, vector4f const & v1)
	{
		return {v0.x - v1.x, v0.y - v1.y, v0.z - v1.z, v0.w - v1.w};
	}

	inline vector4f operator + (vector4f const & v0, vector4f const & v1)
	{
		return {v0.x + v1.x, v0.y + v1.y, v0.z + v1.z, v0.w + v1.w};
	}

	inline vector4f operator * (vector4f const & v0, vector4f const & v1)
	{
		return {v0.x * v1.x, v0.y * v1.y, v0.z * v1.z, v0.w * v1.w};
	}

	inline float det2D(vector4f const & v0, vector4f const & v1)
	{
		return v0.x * v1.y - v0.y * v1.x;
	}

	inline float dot(vector4f const & v0, vector4f const & v1)
	{
		return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z + v0.w * v1.w;
	}

	inline vector4f as_vector(vector3f const & v)
	{
		return {v.x, v.y, v.z, 0.f};
	}

	inline vector4f as_point(vector3f const & v)
	{
		return {v.x, v.y, v.z, 1.f};
	}

	inline vector4f perspective_divide(vector4f v)
	{
		v.w = 1.f / v.w;
		v.x *= v.w;
		v.y *= v.w;
		v.z *= v.w;
		return v;
	}

	inline vector3f to_vector3f(vector4f const & v)
	{
		return {v.x, v.y, v.z};
	}

}
