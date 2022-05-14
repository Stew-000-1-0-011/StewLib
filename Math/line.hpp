/*

StewLib内の全ての図形は端を含む。
交差はそれぞれの図形に含まれる点が存在することとする。

*/

#pragma once

#include <cstdint>

#include "../judge_lower_cost_than_ref.hpp"
#include "vec2d.hpp"

namespace StewLib
{
    namespace Math
    {
        enum class LineType : std::uint8_t
        {
            line,
            segment,
            half
        };

        template<class T>
        struct Line final
        {
            Vec2D<T> center{};
            Vec2D<T> start{};
            Vec2D<T> end{};
            LineType line_type{};

            T start_global() const noexcept
            {
                return start + center;
            }

            T end_global() const noexcept
            {
                return end + center;
            }

            // 直線の方程式
            T line_equation(const low_cost_ref_val_t<Vec2D<T>> vec2d) const noexcept
            {
                // (end - start) / (vec2d - start)
                return (end.x - start.x) * (vec2d.y - (start.y + center.y)) - (end.y - start.y) * (vec2d.x - (start.x + center.x));
            }

            // centerを原点とする直線の方程式
            T line_equation_local(const low_cost_ref_val_t<Vec2D<T>> vec2d) const noexcept
            {
                // (end - start) / (vec2d - start)
                return (end.x - start.x) * (vec2d.y - start.y) - (end.y - start.y) * (vec2d.x - start.x);
            }
        };

        template<class T1, class T2>
        constexpr inline bool is_cross(const low_cost_ref_val_t<Line<T1>> line1, low_cost_ref_val_t<Line<T2>> line2) noexcept
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
}