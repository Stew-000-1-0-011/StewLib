#pragma once

// GNU独自拡張前提(cmath関数のconstexpr化はよ)

/*
要素が二つのベクトル。
諸々の操作を行うとき、例外を吐いてはいけない。
諸々の操作を行うとき、二つの要素の型は同じにならなければならない。
*/

/*
演算子でない関数は、自作クラス型の要素に対しての振る舞いをVec2DFunc内の各クラステンプレートの特殊化で変更できる。
できれば直接にStewLib::Math内の関数を特殊化するのはやめてほしい。
*/

#include <cmath>
#include <utility>
#include <ostream>

#include "cos.hpp"
#include "sin.hpp"

namespace StewLib
{
    namespace
    {
        namespace Math
        {
            template<typename T>
            struct Vec2D final
            {
                using Type = T;
                
                Type x;
                Type y;

#ifdef __cpp_lib_concepts
                template<class Tx, class Ty>
                requires (!is_lower_cost_than_ref<Tx> || !is_lower_cost_than_ref<Ty>)
                constexpr Vec2D(const Tx& x, const Ty& y) noexcept:
                    x{static_cast<Type>(x)},
                    y{static_cast<Type>(y)}
                {}

                template<is_lower_cost_than_ref Tx, is_lower_cost_than_ref Ty>
                constexpr Vec2D(const Tx x, const Ty y) noexcept:
                    x{static_cast<Type>(x)},
                    y{static_cast<Type>(y)}
                {}
#else
                template<class Tx, class Ty>
                constexpr Vec2D(const Tx& x, const Ty& y) noexcept:
                    x{static_cast<Type>(x)},
                    y{static_cast<Type>(y)}
                {}
#endif

                Vec2D() = default;
                Vec2D(const Vec2D&) = default;
                Vec2D& operator=(const Vec2D&) = default;
                Vec2D(Vec2D&&) = default;
                Vec2D& operator=(Vec2D&&) = default;
                ~Vec2D() = default;

                template<typename T2>
                constexpr Vec2D(const Vec2D<T2>& obj) noexcept:
                    x{obj.x},
                    y{obj.y}
                {}

                template<typename T2>
                constexpr Vec2D& operator=(const Vec2D<T2>& obj) noexcept
                    {
                        x = obj.x;
                        y = obj.y;
                        return *this;
                    }

                template<typename T2>
                constexpr Vec2D(Vec2D<T2>&& obj) noexcept:
                    x{std::move(obj.x)},
                    y{std::move(obj.y)}
                {}

                template<typename T2>
                constexpr Vec2D& operator=(Vec2D<T2>&& obj) noexcept
                {
                    x = std::move(obj.x);
                    y = std::move(obj.y);
                    return *this;
                }


                constexpr Vec2D& operator+=(const Vec2D& obj) noexcept
                {
                    x += obj.x;
                    y += obj.y;
                    return *this;
                }

                constexpr Vec2D& operator+=(Vec2D&& obj) noexcept
                {
                    x += std::move(obj.x);
                    y += std::move(obj.y);
                    return *this;
                }

                constexpr Vec2D& operator-=(const Vec2D& obj) noexcept
                {
                    x -= obj.x;
                    y -= obj.y;
                    return *this;
                }

                constexpr Vec2D& operator-=(Vec2D&& obj) noexcept
                {
                    x -= std::move(obj.x);
                    y -= std::move(obj.y);
                    return *this;
                }

                constexpr auto operator-() const noexcept
                {
                    static_assert(std::is_same_v<decltype(-x), decltype(-y)>);
                    return Vec2D<decltype(-x)>(-x, -y);
                }
            };

#ifdef __cpp_lib_concepts
            template<class Tx, class Ty>
            requires (!is_lower_cost_than_ref<Tx> || !is_lower_cost_than_ref<Ty>)
            Vec2D(const Tx&, const Ty&) -> Vec2D<std::common_type_t<Tx, Ty>>;

            template<is_lower_cost_than_ref Tx, is_lower_cost_than_ref Ty>
            Vec2D(const Tx, const Ty) -> Vec2D<std::common_type_t<Tx, Ty>>;
#else
            template<class Tx, class Ty>
            Vec2D(const Tx&, const Ty&) -> Vec2D<std::common_type_t<Tx, Ty>>;
#endif

            template<typename TL, typename TR>
            constexpr inline auto operator+(const Vec2D<TL>& l, const Vec2D<TR>& r) noexcept -> Vec2D<decltype(l.x + r.x)>
            {
                static_assert(std::is_same_v<decltype(l.x + r.x), decltype(l.y + r.y)>);
                return {l.x + r.x, l.y + r.y};
            }

            template<typename TL, typename TR>
            constexpr inline auto operator-(const Vec2D<TL>& l, const Vec2D<TR>& r) noexcept -> Vec2D<decltype(l.x - r.x)>
            {
                static_assert(std::is_same_v<decltype(l.x - r.x), decltype(l.y - r.y)>);
                return {l.x - r.x, l.y - r.y};
            }

            template<typename TL, typename TR>
            constexpr inline auto operator*(const Vec2D<TL>& l, const Vec2D<TR>& r) noexcept
            {
                return l.x * r.x + l.y * r.y;
            }

            template<typename TL, typename TR>
            constexpr inline auto operator*(const TL l, const Vec2D<TR>& r) noexcept -> Vec2D<decltype(l * r.x)>
            {
                static_assert(std::is_same_v<decltype(l * r.x), decltype(l * r.y)>);
                return {l * r.x, l * r.y};
            }

            template<typename TL, typename TR>
            constexpr inline auto operator*(const Vec2D<TL>& l, const TR r) noexcept -> Vec2D<decltype(l.x * r)>
            {
                static_assert(std::is_same_v<decltype(l.x * r), decltype(l.y * r)>);
                return {l.x * r, l.y * r};
            }

            template<typename TL, typename TR>
            constexpr inline auto operator/(const Vec2D<TL>& l, const TR r) noexcept -> Vec2D<decltype(l.x / r)>
            {
                static_assert(std::is_same_v<decltype(l.x / r), decltype(l.y / r)>);
                return {l.x / r, l.y / r};
            }

            template<typename TL, typename TR>
            constexpr inline auto operator/(const Vec2D<TL>& l, const Vec2D<TR>& r) noexcept
            {
                return l.x * r.y - l.y * r.x;
            }


            namespace Vec2DFunc
            {
                template<class T, class Ang>
                struct FuncRot final
                {
                    static constexpr auto rot(const Vec2D<T>& vec2d, const Ang& angle) noexcept
                    {
                        const auto cos_angle = cos(angle);
                        const auto sin_angle = sin(angle);
                        const auto tmp_x = vec2d.x * cos_angle - vec2d.y * sin_angle;
                        const auto tmp_y = vec2d.x * sin_angle + vec2d.y * cos_angle;
                        static_assert(std::is_same_v<decltype(tmp_x), decltype(tmp_y)>);
                        return Vec2D<std::remove_cvref_t<decltype(tmp_x)>>(tmp_x, tmp_y);
                    }
                };

#ifdef __cpp_lib_concepts
                template<class T, is_lower_cost_than_ref Arg>
                struct FuncRot<T, Arg> final
                {
                    static constexpr auto rot(const Vec2D<T>& vec2d, const Arg angle) noexcept
                    {
                        const auto cos_angle = cos<Arg>(angle);
                        const auto sin_angle = sin<Arg>(angle);
                        const auto tmp_x = vec2d.x * cos_angle - vec2d.y * sin_angle;
                        const auto tmp_y = vec2d.x * sin_angle + vec2d.y * cos_angle;
                        static_assert(std::is_same_v<decltype(tmp_x), decltype(tmp_y)>);
                        return Vec2D<std::remove_cvref_t<decltype(tmp_x)>>(tmp_x, tmp_y);
                    }
                };
#endif

                template<class T>
#ifdef __cpp_concepts
                requires std::is_arithmetic_v<T>
#endif
                struct FuncGetAngle final
                {
                    static constexpr auto get_angle(const Vec2D<T>& vec2d) noexcept
                    {
                        /// TODO: GNU拡張の除去
                        return std::atan2(vec2d.y, vec2d.x);
                    }
                };

                // template<class T>
                // struct FuncNorm2 final
                // {
                //     static constexpr auto norm2(const Vec2D<T>& vec2d) noexcept
                //     {
                //         return vec2d.x * vec2d.x + vec2d.y * vec2d.y;
                //     }
                // };

                template<class T>
#ifdef __cpp_concepts
                requires std::is_arithmetic_v<T>
#endif
                struct FuncNorm final
                {
                    static constexpr auto norm(const Vec2D<T>& vec2d) noexcept
                    {
                        return std::sqrt(norm2(vec2d));
                    }
                };
            }

            template<class T, class Ang>
            constexpr inline auto rot(const Vec2D<T>& vec2d, const Ang& angle) noexcept
            {
                return Vec2DFunc::FuncRot<T, Ang>::rot(vec2d, angle);
            }

            template<class T>
            constexpr inline auto get_angle(const Vec2D<T>& vec2d) noexcept
            {
                return Vec2DFunc::FuncGetAngle<T>::get_angle(vec2d);
            }

            // template<class T>
            // constexpr inline auto norm2(const Vec2D<T>& vec2d) noexcept
            // {
            //     return Vec2DFunc::FuncNorm2<T>::norm2(vec2d);
            // }

            template<class T>
            constexpr inline auto norm2(const Vec2D<T>& vec2d) noexcept
            {
                return vec2d.x * vec2d.x + vec2d.y * vec2d.y;
            }

            template<class T>
            constexpr inline auto norm(const Vec2D<T>& vec2d) noexcept
            {
                return Vec2DFunc::FuncNorm<T>::norm(vec2d);
            }

            template<class Traits, class T>
            auto& operator<<(std::basic_ostream<char, Traits>& os, const Vec2D<T>& vec2d)
            {
                return os << "{" << vec2d.x << ", " << vec2d.y << "}";
            }
        }
    }
}