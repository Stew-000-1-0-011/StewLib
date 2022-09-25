/// TODO: n次元に拡張

#pragma once

#include <type_traits>

#include "vec2d.hpp"
#include "local_global.hpp"

namespace StewLib::Math
{
    struct RootOriginBase{};

    template<class T>
    concept IsRootOrigin = std::is_base_of_v<RootOriginBase, T>;

    struct Point2DBase{};

    template<class T>
    concept IsPoint2D = std::is_base_of_v<Point2DBase, T>;

    template<class Elem, class ParentOrRootOrigin>
    class Point2D;

    template<class Elem, IsPoint2D Parent>
    class Point2D final
    {
        const Parent * parent_p;
        Vec2D<Elem> parent_global;
        Vec2D<Elem> self_local;

    public:
        using RootOrigin = typename Parent::RootOrigin;

        constexpr Point2D(const Parent *const parent_p, const Vec2D<Elem>& self_local = {0, 0}) noexcept:
            parent_p{parent_p},
            parent_global{parent_p->get_global()},
            self_local{self_local}
        {}

        template<IsLG LG, class T2>
        constexpr void set(const Vec2D<T2>& vec2d) noexcept
        {
            if constexpr(IsL<LG>)
            {
                self_local = vec2d;
            }
            else
            {
                self_local = vec2d - parent_global;
            }
        }

        template<IsLG LG>
        constexpr auto get() const noexcept
        {
            if constexpr(IsL<LG>)
            {
                return self_local;
            }
            else
            {
                return self_local + parent_global;
            }
        }

        constexpr auto update_parent() noexcept
        {
            return parent_global = parent_p->get<Global>();
        }

        constexpr auto update_all() noexcept
        {
            return (parent_global = parent_p->update_all()) + self_local;
        }
    };

    template<class Elem, IsRootOrigin RootOrigin_>
    class Point2D<Elem, RootOrigin_> final
    {
    public:
        using RootOrigin = RootOrigin_;

        template<IsLG>
        constexpr auto get() const noexcept
        {
            return Vec2D<Elem>{0, 0};
        }

        constexpr auto update_all() const noexcept
        {
            return Vec2D<Elem>{0, 0};
        }
    };
}