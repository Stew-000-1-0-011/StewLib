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
                static constexpr auto sin(const low_cost_ref_val_t<T> x) noexcept
                {
                    /// TODO: GNU拡張の除去
                    return std::sin(x);
                }
            };

            template<typename T>
            inline constexpr auto sin(const low_cost_ref_val_t<T> x) noexcept
            {
                return FuncSin<T>::sin(x);
            }
        }
    }
}