#pragma once

#include <initializer_list>
#include "types.hpp"

namespace math
{
	template <typename T>
	struct vector3d;

	template <typename T>
	struct vector2d
	{
		T x, y;

		constexpr vector2d() = default;

		constexpr vector2d(T val) : x(val), y(val) {}

		constexpr vector2d(std::initializer_list<T> list)
		{
			ASSERT(list.size() == 2, "vector2d must be initialized with 2 elements.");
			auto it = list.begin();
			x = *it++;
			y = *it++;
		}
		
		template <typename U>
		operator vector2d<U>() const
		{
			vector2d<U> ret{ static_cast<U>(x), static_cast<U>(y) };
			return ret;
		}

		template <typename U>
		operator vector3d<U>() const
		{
			vector3d<U> ret{ static_cast<U>(x), static_cast<U>(y), static_cast<U>(0) };
			return ret;
		}

		vector2d<T> operator+=(const vector2d<T>& other)
		{
			x += other.x;
			y += other.y;
			return *this;
		}
	};

	template <typename T>
	inline vector2d<T> operator*(float s, vector2d<T> const& v)
	{
		return { s * v.x, s * v.y };
	}

	template <typename T>
	inline vector2d<T> operator/(vector2d<T> const& v, T s)
	{
		return { v.x / s, v.y / s };
	}

	template <typename T>
	inline vector2d<T> operator*(vector2d<T> const& v, T s)
	{
		return { v.x * s, v.y * s };
	}

	template <typename T>
	inline vector2d<T> operator-(vector2d<T> const& v1, vector2d<T> const& v2)
	{
		return { v1.x - v2.x, v1.y - v2.y };
	}

	template <typename T>
	inline vector2d<T> operator+(vector2d<T> const& v1, vector2d<T> const& v2)
	{
		return { v1.x + v2.x, v1.y + v2.y };
	}

	template <typename T>
	inline vector2d<T> operator*(vector2d<T> const& v1, vector2d<T> const& v2)
	{
		return { v1.x * v2.x, v1.y * v2.y };
	}

	template <typename T>
	inline rnd::f32 dot(const vector2d<T>& v1, const vector2d<T>& v2)
	{
		return v1.x * v2.x + v1.y * v2.y;
	}

	template <typename T>
	static inline rnd::f32 length(const vector2d<T>& v)
	{
		return std::sqrt(dot(v, v));
	}

	template <typename T>
	struct vector4d;

	template <typename T>
	struct vector3d
	{
		T x, y, z;

		constexpr vector3d() = default;

		constexpr vector3d(T val) : x(val), y(val), z(val) {}
		constexpr vector3d(T x, T y, T z) : x(x), y(y), z(z) {}

		constexpr vector3d(std::initializer_list<T> list)
		{
			ASSERT(list.size() == 3, "vector3d must be initialized with 3 elements.");
			auto it = list.begin();
			x = *it++;
			y = *it++;
			z = *it++;
		}

		vector3d operator/(T val) const
		{
			return { x / val, y / val, z / val };
		}

		template <typename U>
		operator vector2d<U>() const
		{
			vector2d<U> ret{ static_cast<U>(x), static_cast<U>(y) };
			return ret;
		}

		template <typename U>
		operator vector4d<U>() const
		{
			vector4d<U> ret{ static_cast<U>(x), static_cast<U>(y), static_cast<U>(z), static_cast<U>(0) };
			return ret;
		}
	};

	template <typename T>
	inline vector3d<T> operator*(float s, vector3d<T> const& v)
	{
		return { s * v.x, s * v.y, s * v.z };
	}

	template <typename T>
	inline vector3d<T> operator-(vector3d<T> const& v)
	{
		return { -v.x, -v.y, -v.z };
	}

	template <typename T>
	inline vector3d<T> operator*(vector3d<T> const& v, float s)
	{
		return { s * v.x, s * v.y, s * v.z };
	}

	template <typename T>
	inline vector3d<T> operator/(vector3d<T> const& v, float s)
	{
		return { v.x / s, v.y / s, v.z / s };
	}

	template <typename T>
	inline vector3d<T> operator-(vector3d<T> const& v1, vector3d<T> const& v2)
	{
		return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
	}

	template <typename T>
	inline vector3d<T> operator+(vector3d<T> const& v1, vector3d<T> const& v2)
	{
		return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
	}

	template <typename T>
	inline vector3d<T> operator*(vector3d<T> const& v1, vector3d<T> const& v2)
	{
		return { v1.x * v2.x, v1.y * v2.y, v1.z * v2.z };
	}

	template <typename T>
	inline rnd::f32 dot(const vector3d<T>& v1, const vector3d<T>& v2)
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	template <typename T>
	static inline rnd::f32 length(const vector3d<T>& v)
	{
		return std::sqrt(dot(v, v));
	}

	template <typename T>
	static inline vector3d<T> normalize(const vector3d<T>& v)
	{
		return v / length(v);
	}

	template <typename T>
	static inline vector3d<T> cross(const vector3d<T>& v1, const vector3d<T>& v2)
	{
		return { v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.z };
	}

	template <typename T>
	struct vector4d
	{
		T x, y, z, w;

		vector4d(T val) : x(val), y(val), z(val), w(val) {}

		vector4d() = default;

		vector4d(std::initializer_list<T> list)
		{
			ASSERT(list.size() == 4, "vector4d must be initialized with 4 elements.");
			auto it = list.begin();
			x = *it++;
			y = *it++;
			z = *it++;
			w = *it++;
		}

		vector4d(vector3d<T> v, T s)
			:
			x(v.x), y(v.y), z(v.z), w(s)
		{}

		vector4d operator/(T val) const
		{
			return { x / val, y / val, z / val, w / val };
		}

		vector4d<T>& operator+=(const vector4d<T>& other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
			w += other.w;

			return *this;
		}

		template <typename U>
		operator vector3d<U>() const
		{
			vector3d<U> ret{ static_cast<U>(x), static_cast<U>(y), static_cast<U>(z) };
			return ret;
		}
	};

	template <typename T>
	inline vector4d<T> operator*(float s, vector4d<T> const& v)
	{
		return { s * v.x, s * v.y, s * v.z, s * v.w };
	}

	template <typename T>
	inline vector4d<T> operator/(vector4d<T> const& v, float s)
	{
		return { v.x / s, v.y / s, v.z / s, v.w / s };
	}

	template <typename T>
	inline vector4d<T> operator*(vector4d<T> const& v, float s)
	{
		return { v.x * s, v.y * s, v.z * s, v.w * s };
	}

	template <typename T>
	inline vector4d<T> operator-(vector4d<T> const& v0, vector4d<T> const& v1)
	{
		return { v0.x - v1.x, v0.y - v1.y, v0.z - v1.z, v0.w - v1.w };
	}

	template <typename T>
	inline vector4d<T> operator+(vector4d<T> const& v0, vector4d<T> const& v1)
	{
		return { v0.x + v1.x, v0.y + v1.y, v0.z + v1.z, v0.w + v1.w };
	}

	template <typename T>
	inline vector4d<T> operator*(vector4d<T> const& v0, vector4d<T> const& v1)
	{
		return { v0.x * v1.x, v0.y * v1.y, v0.z * v1.z, v0.w * v1.w };
	}

	template <typename T>
	inline rnd::f32 dot(const vector4d<T>& v1, const vector4d<T>& v2)
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
	}

	template <typename T>
	inline vector3d<T> reflect(const vector3d<T>& I, const vector3d<T>& N)
	{
		return I - N * dot(N, I) * (T)2;
	}

	using vec2  = vector2d<rnd::f32>;
	using vec2d = vector2d<rnd::f64>;
	using vec2i = vector2d<rnd::i32>;

	using vec3  = vector3d<rnd::f32>;
	using vec3d = vector3d<rnd::f64>;
	using vec3i = vector3d<rnd::i32>;

	using vec4  = vector4d<rnd::f32>;
	using vec4d = vector4d<rnd::f64>;
	using vec4i = vector4d<rnd::i32>;
}
