#pragma once

#include <type_traits>
#include <concepts>

namespace StewLib
{
    inline constexpr struct SmartObjectMoveConstructTag
    {} smart_object_move_cponstract_tag;

    template<class T>
    requires std::is_object_v<T>
    class SmartObject final
    {
        T * dp;

    public:
        constexpr SmartObject(auto&& ... args):
            dp{new T{static_cast<decltype(args)&&>(args) ...}}
        {}

        constexpr SmartObject(SmartObjectMoveConstructTag, T *const dp) noexcept:
            dp{dp}
        {}

        constexpr SmartObject():
            dp{new T{}}
        {}

        template<class T2>
        constexpr SmartObject(const SmartObject<T2>& obj):
            dp{new T{*obj.dp}}
        {}

        template<class T2>
        constexpr SmartObject(SmartObject<T2>&& obj):
            dp{new T{std::move(*obj.dp)}}
        {}

        template<class T2>
        constexpr SmartObject& operator =(const SmartObject<T2>& obj)
        {
            return *dp = *obj.dp;
        }

        template<class T2>
        constexpr SmartObject& operator =(SmartObject<T2>&& obj)
        {
            return *dp = std::move(*obj.dp);
        }

        constexpr ~SmartObject()
        {
            delete dp;
        }

        constexpr auto operator ->() const noexcept
        {
            if constexpr(std::is_pointer_v<T>)
            {
                return *dp;
            }
            else
            {
                return dp;
            }
        }

        // constexpr auto operator &() const noexcept
        // {
        //     return dp;
        // }

        constexpr operator T&() const noexcept
        {
            return *dp;
        }

        constexpr auto ptr() const noexcept
        {
            return dp;
        }

        constexpr auto move_ptr() noexcept
        {
            const auto ret = dp;
            dp = nullptr;
            return ret;
        }
    };
}