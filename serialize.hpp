#pragma once

#include <cstdint>
#include <cstring>

#include "judge_lower_cost_than_ref.hpp"

namespace StewLib
{
    namespace
    {
        template<std::size_t unit_size_, class RawData_>
        struct Serialize final
        {
            using RawData = RawData_;
            constexpr static std::size_t unit_size = unit_size_;

            static constexpr std::size_t chunks_size = sizeof(RawData) / unit_size_ + (sizeof(RawData) % unit_size)? 1 : 0;
            static constexpr std::uint8_t last_size = (sizeof(RawData) % unit_size) ? (sizeof(RawData) % unit_size) : 8;

            std::uint8_t chunks[chunks_size][unit_size]{};

            Serialize(const low_cost_ref_val_t<RawData> raw_data)
            {
                std::memcpy(chunks, &raw_data, sizeof(RawData));
            }
        };

    }
}