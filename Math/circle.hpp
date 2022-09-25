/// TODO: n次元に拡張、LocalGlobalに対応

ｗ#pragma once

#include "../noexcept_auto.hpp"

namespace StewLib::Math
{
    template<class Point>
    struct Circle final
    {
        Point center{};
        typename Point::Type range{};

        template<>
        constexpr bool is_in(const Point& point) const Stew_noexcept_auto
        {
            return norm2(point.get() - center.get()) <= range * range;
        }
    };
}