#pragma once

#include <cstddef>

namespace StewLib
{
    // NOT Thread Safe
    class RefCount final
    {
        std::size_t * count_dp{};

    public:
        // null
        RefCount() = default;

        // if count is zero, null. else, non-null.
        constexpr RefCount(const std::size_t count) noexcept:
            count_dp{count ? (new std::size_t(count)) : nullptr}
        {}

        constexpr RefCount(const RefCount& obj) noexcept:
            count_dp{obj.count_dp}
        {
            if(count_dp) ++*count_dp;
        }

        constexpr RefCount(RefCount&& obj) noexcept:
            count_dp{obj.count_dp}
        {
            obj.count_dp = nullptr;
        }

        constexpr RefCount& operator =(const RefCount& obj) noexcept
        {
            if(count_dp && !--*count_dp) delete count_dp;
            count_dp = obj.count_dp;
            if(count_dp) ++*count_dp;
            return *this;
        }

        constexpr RefCount& operator =(RefCount&& obj) noexcept
        {
            if(count_dp && !--*count_dp) delete count_dp;
            count_dp = obj.count_dp;
            obj.count_dp = nullptr;
            return *this;
        }

        constexpr ~RefCount()
        {
            if(count_dp)
            {
                --*count_dp;
                if(!*count_dp) delete count_dp;
            }
        }

        constexpr operator std::size_t() const noexcept
        {
            if(count_dp) return *count_dp;
            else return 0;
        }
    };
}