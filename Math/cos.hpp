/*

余裕があればGNU拡張をどうにかする。

*/


#pragma once

#include <cmath>

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
#ifdef __cpp_lib_concepts
                static constexpr auto cos(const low_cost_ref_val_t<T> x) noexcept
#else
                static constexpr auto cos(const T& x) noexcept
#endif
                {
                    /// TODO: GNU拡張の除去
                    return std::cos(x);
                }
            };

            template<typename T>
#ifdef __cpp_lib_concepts
            inline constexpr auto cos(const low_cost_ref_val_t<T> x) noexcept
#else
            inline constexpr auto cos(const T& x) noexcept
#endif
            {
                return FuncCos<T>::cos(x);
            }
        }
    }
}