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
            struct FuncSin final
            {
                static constexpr auto sin(const double x) noexcept
                {
                    /// TODO: GNU拡張の除去
                    return std::sin(x);
                }
            };

            template<class T>
            requires std::floating_point<T> && is_lower_cost_than_ref<T>
            struct FuncSin<T> final
            {
                static constexpr auto sin(const T x) noexcept
                {
                    /// TODO: GNU拡張の除去
                    return std::sin(x);
                }
            };

            template<class T>
            requires std::floating_point<T>
            struct FuncSin<T> final
            {
                static constexpr auto sin(const T x) noexcept
                {
                    /// TODO: GNU拡張の除去
                    return std::sin(x);
                }
            };

            template<typename T>
            inline constexpr auto sin(const T& x) noexcept
            {
                return FuncSin<T>::sin(x);
            }
        }
    }
}