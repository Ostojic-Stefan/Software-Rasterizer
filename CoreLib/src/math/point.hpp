#pragma once

#include "pch.h"
#include "types.hpp"

namespace math
{
	template <typename T>
	struct point3d;

	template <typename T>
	struct point2d
	{
		T x, y;

		template <typename U>
		operator point2d<U>() const
		{
			point2d<U> ret{ static_cast<U>(x), static_cast<U>(y) };
			return ret;
		}

		template <typename U>
		operator point3d<U>() const
		{
			point3d<U> ret{ static_cast<U>(x), static_cast<U>(y), static_cast<U>(0) };
			return ret;
		}

		point2d<T>& operator+=(const point2d<T>& other)
		{
			x += other.x;
			y += other.y;
			return *this;
		}

		point2d<T>& operator-=(const point2d<T>& other)
		{
			x -= other.x;
			y -= other.y;
			return *this;
		}

		point2d<T>& operator*=(const point2d<T>& other)
		{
			x *= other.x;
			y *= other.y;
			return *this;
		}

		point2d<T>& operator/=(const point2d<T>& other)
		{
			x /= other.x;
			y /= other.y;
			return *this;
		}
	};

	template<typename T>
	static inline point2d<T> operator+(const point2d<T>& a, const point2d<T>& b)
	{
		return { a.x + b.x, a.y + b.y };
	}

	template<typename T>
	static inline point2d<T> operator-(const point2d<T>& a, const point2d<T>& b)
	{
		return { a.x - b.x, a.y - b.y };
	}

	template<typename T>
	static inline point2d<T> operator*(const point2d<T>& a, const point2d<T>& b)
	{
		return { a.x * b.x, a.y * b.y };
	}

	template<typename T>
	static inline point2d<T> operator/(const point2d<T>& a, const point2d<T>& b)
	{
		return { a.x / b.x, a.y / b.y };
	}

	template<typename T>
	static inline point2d<T> operator/(const point2d<T>& a, T s)
	{
		return { a.x / s, a.y / s };
	}

	template<typename T>
	static inline point2d<T> operator-(const point2d<T>& a, T s)
	{
		return { a.x - s, a.y - s };
	}

	template<typename T>
	static inline point2d<T> clamp(const point2d<T>& val, const point2d<T>& min, const point2d<T>& max)
	{
		return point2d<T>{
			.x = std::clamp(val.x, min.x, max.x),
			.y = std::clamp(val.y, min.y, max.y),
		};
	}

	template <typename T>
	struct point3d
	{
		T x, y, z;

		template <typename U>
		operator point2d<U>() const
		{
			point2d<U> ret{ static_cast<U>(x), static_cast<U>(y) };
			return ret;
		}

		template <typename U>
		operator point3d<U>() const
		{
			point3d<U> ret{ static_cast<U>(x), static_cast<U>(y), static_cast<U>(z) };
			return ret;
		}
	};


	using pt2  = point2d<rnd::f32>;
	using pt2d = point2d<rnd::f64>;
	using pt2i = point2d<rnd::i32>;

	using pt3  = point3d<rnd::f32>;
	using pt3d = point3d<rnd::f64>;
	using pt3i = point3d<rnd::i32>;


}
