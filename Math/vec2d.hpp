#pragma once

// GNU独自拡張前提(cmath関数のconstexpr化はよ)
// ↑errnoあたりの仕様のせいで難しいらしい(定数式評価か否かで分岐させればいいのでは？)

// メタクラスが使えるようになったら多分もっと楽に記述できそう。

// 型に対する制約を何かしら付けるべきなんだろうなぁ...

#include <cmath>
#include <utility>
#include <ostream>

#include "../noexcept_auto.hpp"
#include "functions.hpp"

namespace StewLib::Math
{
    template<class T>
    struct Vec2D final
    {
        using Type = T;
        
        T x;
        T y;

        template<class Tx, class Ty>
        constexpr Vec2D(const Tx& x, const Ty& y) Stew_noexcept_auto:
            x{static_cast<T>(x)},
            y{static_cast<T>(y)}
        {}

        Vec2D() = default;
        Vec2D(const Vec2D&) = default;
        Vec2D& operator =(const Vec2D&) = default;
        Vec2D(Vec2D&&) = default;
        Vec2D& operator =(Vec2D&&) = default;
        ~Vec2D() = default;

        template<typename T2>
        constexpr Vec2D(const Vec2D<T2>& obj) Stew_noexcept_auto:
            x{obj.x},
            y{obj.y}
        {}

        template<typename T2>
        constexpr Vec2D& operator =(const Vec2D<T2>& obj) Stew_noexcept_auto
        {
            x = obj.x;
            y = obj.y;
            return *this;
        }

        template<typename T2>
        constexpr Vec2D(Vec2D<T2>&& obj) Stew_noexcept_auto:
            x{std::move(obj.x)},
            y{std::move(obj.y)}
        {}

        template<typename T2>
        constexpr Vec2D& operator =(Vec2D<T2>&& obj) Stew_noexcept_auto
        {
            x = std::move(obj.x);
            y = std::move(obj.y);
            return *this;
        }

        template<class T2>
        constexpr Vec2D& operator +=(const Vec2D<T2>& obj) Stew_noexcept_auto
        {
            x += obj.x;
            y += obj.y;
            return *this;
        }

        template<class T2>
        constexpr Vec2D& operator -=(const Vec2D<T2>& obj) Stew_noexcept_auto
        {
            x -= obj.x;
            y -= obj.y;
            return *this;
        }

        template<class T2>
        constexpr Vec2D& operator *=(const Vec2D<T2>& obj) Stew_noexcept_auto
        {
            x *= obj.x;
            y *= obj.y;
            return *this;
        }
        
        template<class T2>
        constexpr Vec2D& operator /=(const Vec2D<T2>& obj) Stew_noexcept_auto
        {
            x /= obj.x;
            y /= obj.y;
            return *this;
        }

        friend constexpr auto operator-(const Vec2D& obj) const Stew_noexcept_auto
        {
            return Vec2D<decltype(-obj.x)>(-obj.x, -obj.y);
        }

        template<class T2>
        friend constexpr auto operator +(const Vec2D& l, const Vec2D<T2>& r) Stew_noexcept_auto
        {
            return Vec2D{l.x + r.x, l.y + r.y};
        }

        template<class T2>
        friend constexpr auto operator -(const Vec2D& l, const Vec2D<T2>& r) Stew_noexcept_auto
        {
            return Vec2D{l.x - r.x, l.y - r.y};
        }

        template<class T2>
        friend constexpr auto operator *(const T2& l, const Vec2D& r) Stew_noexcept_auto
        {
            return Vec2D{l * r.x, l * r.y};
        }

        template<class T2>
        friend constexpr auto operator *(const Vec2D& l, const T2& r) Stew_noexcept_auto
        {
            return Vec2D{l.x * r, l.y * r};
        }

        template<class T2>
        friend constexpr auto operator /(const Vec2D& l, const T2& r) Stew_noexcept_auto
        {
            return Vec2D{l.x / r, l.y / r};
        }

        template<class Traits, class T2>
        friend constexpr auto& operator <<(std::basic_ostream<char, Traits>& os, const Vec2D<T2>& vec2d)
        {
            return os << "{" << vec2d.x << ", " << vec2d.y << "}";
        }

        friend constexpr auto get_angle(const Vec2D& obj) const Stew_noexcept_auto
        {
            return atan2(obj.y, obj.x);
        }

        friend constexpr auto norm2(const Vec2D& obj) const Stew_noexcept_auto
        {
            return obj.x * obj.x + obj.y * obj.y;
        }

        friend constexpr auto norm(const Vec2D& obj) const Stew_noexcept_auto
        {
            return sqrt(norm2(obj));
        }

        template<class T2>
        friend constexpr auto dot(const Vec2D& l, const Vec2D<T2>& r) Stew_noexcept_auto
        {
            return l.x * r.x + l.y * r.y;
        }

        template<class T2>
        friend constexpr auto cross_norm(const Vec2D& l, const Vec2D<T2>& r) Stew_noexcept_auto
        {
            return l.x * r.y - l.y * r.x;
        }

        template<class T2>
        friend constexpr auto rot(const Vec2D& vec2d, const T2& angle) Stew_noexcept_auto
        {
            const auto cos_angle = cos(angle);
            const auto sin_angle = sin(angle);
            return Vec2D{vec2d.x * cos_angle - vec2d.y * sin_angle, vec2d.x * sin_angle + vec2d.y * cos_angle};
        }
    };

    template<class Tx, class Ty>
    Vec2D(const Tx&, const Ty&) -> Vec2D<std::common_type_t<Tx, Ty>>;
}