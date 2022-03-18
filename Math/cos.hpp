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
                static constexpr auto cos(const low_cost_ref_val_t<T> x) noexcept
                {
                    /// TODO: GNU拡張の除去
                    return std::cos(x);
                }
            };

            template<typename T>
            inline constexpr auto cos(const low_cost_ref_val_t<T> x) noexcept
            {
                return FuncCos<T>::cos(x);
            }
        }
    }
}