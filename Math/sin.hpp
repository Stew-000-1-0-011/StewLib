/*

余裕があればGNU拡張をどうにかする。

*/


#pragma once

#include <cmath>

#include "../judge_lower_cost_than_ref.hpp"


namespace StewLib
{
    namespace Math
    {
        template<class T>
        struct FuncSin final
        {
#ifdef __cpp_lib_concepts
            static constexpr auto sin(const low_cost_ref_val_t<T> x) noexcept
#else
            static constexpr auto sin(const T& x) noexcept
#endif
            {
                /// TODO: GNU拡張の除去
                return std::sin(x);
            }
        };

        template<typename T>
#ifdef __cpp_lib_concepts
        inline constexpr auto sin(const low_cost_ref_val_t<T> x) noexcept
#else
        inline constexpr auto sin(const T& x) noexcept
#endif
        {
            return FuncSin<T>::sin(x);
        }
    }
}
