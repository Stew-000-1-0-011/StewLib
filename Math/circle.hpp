#pragma once

#include "vec2d.hpp"

namespace StewLib
{
    namespace
    {
        namespace Math
        {
            template<typename T>
            struct Circle final
            {
                Vec2D<T> center{};
                T range{};

                constexpr bool is_in(const low_cost_ref_val_t<Vec2D<T>> point) const noexcept
                {
                    return norm2(point - center) <= range * range;
                }
            };
        }
    }
}