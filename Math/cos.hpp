/*

余裕があればGNU拡張をどうにかする。

*/


#pragma once

#include <cmath>
#include <concepts>

#include "../judge_lower_cost_than_ref.hpp"


namespace StewLib
{
    namespace
    {
        namespace Math
        {
            template<class T>
            struct FuncCos final
            {
                static constexpr auto cos(const double x) noexcept
                {
                    /// TODO: GNU拡張の除去
                    return std::cos(x);
                }
            };

            template<class T>
            requires std::floating_point<T> && is_lower_cost_than_ref<T>
            struct FuncCos<T> final
            {
                static constexpr auto cos(const T x) noexcept
                {
                    /// TODO: GNU拡張の除去
                    return std::cos(x);
                }
            };

            template<class T>
            requires std::floating_point<T>
            struct FuncCos<T> final
            {
                static constexpr auto cos(const T& x) noexcept
                {
                    /// TODO: GNU拡張の除去
                    return std::cos(x);
                }
            };

            template<typename T>
            inline constexpr auto cos(const T& x) noexcept
            {
                return FuncCos<T>::cos(x);
            }
        }
    }
}