#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

#include <execution>
#include <algorithm>

#include "judge_lower_cost_than_ref.hpp"

namespace StewLib
{
    namespace
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
                std::reverse(std::execution::par_unseq, buffer, buffer + size);

                is_reversed = !is_reversed;
            }

            operator T() const noexcept
            {
                T ret;
                std::memcpy(&ret, buffer, sizeof(T));
                return ret;
            }
        };
    }
}