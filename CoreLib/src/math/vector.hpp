#pragma once

#include <initializer_list>
#include <cmath>
#include "types.hpp"
#include <cassert>
#include <immintrin.h>
#include <print>
#include "core.hpp"

namespace math
{
    template <typename T>
    struct vector2d;

    template <typename T>
    struct vector3d;

    template <typename T>
    struct vector4d;

    template <typename T>
    struct vector2d
    {
        union
        {
            struct { T x, y; };
            T values[2];
        };

        constexpr vector2d()
            : x(T(0)), y(T(0))
        {}

        constexpr explicit vector2d(T val)
            : x(val), y(val)
        {}

        constexpr vector2d(T _x, T _y)
            : x(_x), y(_y)
        {}

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
            return vector2d<U>{ static_cast<U>(x), static_cast<U>(y) };
        }

        template <typename U>
        operator vector3d<U>() const
        {
            return vector3d<U>{ static_cast<U>(x), static_cast<U>(y), static_cast<U>(0) };
        }

        vector2d<T>& operator+=(const vector2d<T>& other)
        {
            x += other.x;
            y += other.y;
            return *this;
        }

        vector2d<T>& operator-=(const vector2d<T>& other)
        {
            x -= other.x;
            y -= other.y;
            return *this;
        }

        vector2d<T>& operator*=(T scalar)
        {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        vector2d<T>& operator/=(T scalar)
        {
            x /= scalar;
            y /= scalar;
            return *this;
        }
    };

    template <typename T>
    inline vector2d<T> operator+(vector2d<T> const& a, vector2d<T> const& b)
    {
        return { a.x + b.x, a.y + b.y };
    }

    template <typename T>
    inline vector2d<T> operator-(vector2d<T> const& a, vector2d<T> const& b)
    {
        return { a.x - b.x, a.y - b.y };
    }

    template <typename T>
    inline vector2d<T> operator-(vector2d<T> const& v)
    {
        return { -v.x, -v.y };
    }

    template <typename T>
    inline vector2d<T> operator*(vector2d<T> const& v, T s)
    {
        return { v.x * s, v.y * s };
    }

    template <typename T>
    inline vector2d<T> operator*(T s, vector2d<T> const& v)
    {
        return { v.x * s, v.y * s };
    }

    template <typename T>
    inline vector2d<T> operator/(vector2d<T> const& v, T s)
    {
        return { v.x / s, v.y / s };
    }

    template <typename T>
    inline vector2d<T> operator*(vector2d<T> const& a, vector2d<T> const& b)
    {
        return { a.x * b.x, a.y * b.y };
    }

    template <typename T>
    inline vector2d<T> operator/(vector2d<T> const& a, vector2d<T> const& b)
    {
        return { a.x / b.x, a.y / b.y };
    }

    template <typename T>
    inline T dot(const vector2d<T>& a, const vector2d<T>& b)
    {
        return (a.x * b.x) + (a.y * b.y);
    }

    template <typename T>
    inline T length(const vector2d<T>& v)
    {
        return std::sqrt(dot(v, v));
    }

    template <typename T>
    inline vector2d<T> normalize(const vector2d<T>& v)
    {
        T len = length(v);
        assert(len != T(0) && "Cannot normalize a zero-length vector2d");
        return v / len;
    }

    template <typename T>
    struct vector3d
    {
        union
        {
            struct { T x, y, z; };
            T values[3];
        };

        constexpr vector3d()
            : x(T(0)), y(T(0)), z(T(0))
        {}

        constexpr explicit vector3d(T val)
            : x(val), y(val), z(val)
        {}

        constexpr vector3d(T _x, T _y, T _z)
            : x(_x), y(_y), z(_z)
        {}

        constexpr vector3d(std::initializer_list<T> list)
        {
            assert(list.size() == 3 && "vector3d must be initialized with 3 elements.");
            auto it = list.begin();
            x = *it++;
            y = *it++;
            z = *it++;
        }

        template <typename U>
        operator vector2d<U>() const
        {
            return vector2d<U>{ static_cast<U>(x), static_cast<U>(y) };
        }

        template <typename U>
        operator vector4d<U>() const
        {
            return vector4d<U>{ static_cast<U>(x),
                static_cast<U>(y),
                static_cast<U>(z),
                static_cast<U>(0) };
        }

        vector3d<T>& operator+=(const vector3d<T>& other)
        {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }

        vector3d<T>& operator-=(const vector3d<T>& other)
        {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }

        vector3d<T>& operator*=(T scalar)
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }

        vector3d<T>& operator/=(T scalar)
        {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            return *this;
        }

        vector3d<T> operator/(T scalar) const
        {
            return { x / scalar, y / scalar, z / scalar };
        }
    };

    template <typename T>
    inline vector3d<T> operator+(vector3d<T> const& a, vector3d<T> const& b)
    {
        return { a.x + b.x, a.y + b.y, a.z + b.z };
    }

    template <typename T>
    inline vector3d<T> operator-(vector3d<T> const& a, vector3d<T> const& b)
    {
        return { a.x - b.x, a.y - b.y, a.z - b.z };
    }

    template <typename T>
    inline vector3d<T> operator-(vector3d<T> const& v)
    {
        return { -v.x, -v.y, -v.z };
    }

    template <typename T>
    inline vector3d<T> operator*(vector3d<T> const& v, T s)
    {
        return { v.x * s, v.y * s, v.z * s };
    }

    template <typename T>
    inline vector3d<T> operator*(T s, vector3d<T> const& v)
    {
        return { v.x * s, v.y * s, v.z * s };
    }

    template <typename T>
    inline vector3d<T> operator/(vector3d<T> const& v, T s)
    {
        return { v.x / s, v.y / s, v.z / s };
    }

    template <typename T>
    inline vector3d<T> operator*(vector3d<T> const& a, vector3d<T> const& b)
    {
        return { a.x * b.x, a.y * b.y, a.z * b.z };
    }

    template <typename T>
    inline T dot(const vector3d<T>& a, const vector3d<T>& b)
    {
        return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
    }

    template <typename T>
    inline T length(const vector3d<T>& v)
    {
        return std::sqrt(dot(v, v));
    }

    template <typename T>
    inline vector3d<T> normalize(const vector3d<T>& v)
    {
        T len = length(v);
        assert(len != T(0) && "Cannot normalize a zero-length vector3d");
        return v / len;
    }

    template <typename T>
    inline vector3d<T> cross(const vector3d<T>& a, const vector3d<T>& b)
    {
        return {
            (a.y * b.z) - (a.z * b.y),
            (a.z * b.x) - (a.x * b.z),
            (a.x * b.y) - (a.y * b.x)
        };
    }

    template <typename T>
    inline vector3d<T> reflect(const vector3d<T>& I, const vector3d<T>& N)
    {
        return I - N * (dot(N, I) * T(2));
    }

    template <typename T>
    struct vector4d
    {
        union
        {
            struct { T x, y, z, w; };
            T values[4];
            __m128 _v;
        };

        constexpr vector4d()
            : x(T(0)), y(T(0)), z(T(0)), w(T(0))
        {}

        constexpr explicit vector4d(T val)
            : x(val), y(val), z(val), w(val)
        {}

        constexpr vector4d(std::initializer_list<T> list)
        {
            assert(list.size() == 4 && "vector4d must be initialized with 4 elements.");
            auto it = list.begin();
            x = *it++;
            y = *it++;
            z = *it++;
            w = *it++;
        }

        constexpr vector4d(const vector3d<T>& v, T _w)
            : x(v.x), y(v.y), z(v.z), w(_w)
        {}

        template <typename U>
        operator vector3d<U>() const
        {
            return vector3d<U>{ static_cast<U>(x),
                static_cast<U>(y),
                static_cast<U>(z) };
        }

        vector4d<T>& operator+=(const vector4d<T>& other)
        {
            x += other.x;
            y += other.y;
            z += other.z;
            w += other.w;
            return *this;
        }

        vector4d<T>& operator-=(const vector4d<T>& other)
        {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            w -= other.w;
            return *this;
        }

        vector4d<T>& operator*=(T scalar)
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            w *= scalar;
            return *this;
        }

        vector4d<T>& operator/=(T scalar)
        {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            w /= scalar;
            return *this;
        }

        vector4d<T> operator/(T scalar) const
        {
            return { x / scalar, y / scalar, z / scalar, w / scalar };
        }
    };

    template <typename T>
    inline vector4d<T> operator+(vector4d<T> const& a, vector4d<T> const& b)
    {
        return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
    }

    template <typename T>
    inline vector4d<T> operator-(vector4d<T> const& a, vector4d<T> const& b)
    {
        return { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
    }

    template <typename T>
    inline vector4d<T> operator-(vector4d<T> const& v)
    {
        return { -v.x, -v.y, -v.z, -v.w };
    }

    template <typename T>
    inline vector4d<T> operator*(vector4d<T> const& v, T s)
    {
        return { v.x * s, v.y * s, v.z * s, v.w * s };
    }

    template <typename T>
    inline vector4d<T> operator*(T s, vector4d<T> const& v)
    {
        return { v.x * s, v.y * s, v.z * s, v.w * s };
    }

    template <typename T>
    inline vector4d<T> operator/(vector4d<T> const& v, T s)
    {
        return { v.x / s, v.y / s, v.z / s, v.w / s };
    }

    template <typename T>
    inline vector4d<T> operator*(vector4d<T> const& a, vector4d<T> const& b)
    {
        return { a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w };
    }

    template <typename T>
    inline T dot(const vector4d<T>& a, const vector4d<T>& b)
    {
        return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
    }

    using vec2 = vector2d<rnd::f32>;
    using vec2d = vector2d<rnd::f64>;
    using vec2i = vector2d<rnd::i32>;

    using vec3 = vector3d<rnd::f32>;
    using vec3d = vector3d<rnd::f64>;
    using vec3i = vector3d<rnd::i32>;

    using vec4 = vector4d<rnd::f32>;
    using vec4d = vector4d<rnd::f64>;
    using vec4i = vector4d<rnd::i32>;
}
