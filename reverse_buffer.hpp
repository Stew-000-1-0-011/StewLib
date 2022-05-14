// C++23以降はstd::bitswapを用いること。

#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

#include <execution>
#include <algorithm>

#include "judge_lower_cost_than_ref.hpp"
#include "static_warn.hpp"

#if __cplusplus >= 202300L
Stew_static_warn("Stew: you should use std::bitswap.");
#endif

namespace StewLib
{
    template<typename T>
    struct ReverseBuffer final
    {
        constexpr static std::size_t size = sizeof(T);
        std::byte buffer[size]{};
        bool is_reversed{false};

        ReverseBuffer() = default;
        ReverseBuffer(const ReverseBuffer&) = default;
        ReverseBuffer& operator=(const ReverseBuffer&) = default;
        ReverseBuffer(ReverseBuffer&&) = default;
        ReverseBuffer& operator=(ReverseBuffer&&) = default;

        ReverseBuffer(const low_cost_ref_val_t<T> obj) noexcept
        {
            std::memcpy(buffer, &obj, size);
        }

        void reverse() noexcept
        {
            if constexpr(size > /* TODO: */100)
            {
                std::reverse(std::execution::par_unseq, buffer, buffer + size);
            }
            else
            {
                std::reverse(std::execution::seq, buffer, buffer + size);
            }

            is_reversed = !is_reversed;
        }

        explicit operator T() const noexcept
        {
            T ret;
            std::memcpy(&ret, buffer, sizeof(T));
            return ret;
        }
    };
}