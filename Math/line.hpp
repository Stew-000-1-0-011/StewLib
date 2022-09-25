/*

StewLib内の全ての図形は端を含む。
交差はそれぞれの図形に含まれる点が存在することとする。

*/

/// TODO: n次元に拡張、LocalGlobalに対応


#pragma once

#include <cstdint>

#include "vec2d.hpp"

namespace StewLib::Math
{
    enum class LineType : std::uint8_t
    {
        line,
        segment,
        half
    };

    template<class Vec>
    struct Line final
    {
        Vec center{};  // global
        Vec start{};  // local
        Vec2D<T> end{};  // local
        LineType line_type{};

        constexpr auto start_global() const noexcept
        {
            return start + center;
        }

        constexpr auto end_global() const noexcept
        {
            return end + center;
        }

        // 直線の方程式
        template<class T2>
        constexpr auto line_equation(const Vec2D<T2>& vec2d) const noexcept
        {
            const auto g_start = start + center;
            return (end - g_start) / (vec2d - g_start);
        }

        // centerを原点とする直線の方程式
        constexpr auto line_equation_local(const Vec2D<T2>& vec2d) const noexcept
        {
            return (end - start) / (vec2d - start);
        }
    };

    template<class T1, class T2>
    constexpr inline bool is_cross(const Line<T1>& line1, const Line<T2>& line2) noexcept
    {
        switch(line1.line_type)
        {
        case LineType::line:
            // 直線で分けられた平面の異なる側にあるかどうか
            return (line2.line_equation(line1.start_global()) * line2.line_equation(line1.end_global())) <= 0;
            
        case LineType::segment:
        case LineType::half:
            return ((line2.line_equation(line1.start_global()) * line2.line_equation(line1.end_global())) <= 0) &&
                ((line1.line_equation(line2.start_global()) * line1.line_equation(line2.end_global())) <= 0);
        }
    }

    // centerを考慮しない。原点が一致する場合はこっちを用いる。
    template<class T1, class T2>
    constexpr inline bool is_cross_l(const low_cost_ref_val_t<Line<T1>> line1, low_cost_ref_val_t<Line<T2>> line2) noexcept
    {
        switch(line1.line_type)
        {
        case LineType::line:
            // 直線で分けられた平面の異なる側にあるかどうか
            return (line2.line_equation_l(line1.start) * line2.line_equation_l(line1.end)) <= 0;
            
        case LineType::segment:
        case LineType::half:
            return ((line2.line_equation_l(line1.start) * line2.line_equation_l(line1.end)) <= 0) &&
                ((line1.line_equation_l(line2.start) * line1.line_equation_l(line2.end)) <= 0);
        }
    }
}