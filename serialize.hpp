#pragma once

#include <cstdint>
#include <cstring>

#include "judge_lower_cost_than_ref.hpp"

namespace StewLib
{
    namespace
    {
        template<class RawData_, std::size_t unit_size_>
        struct Serialize final
        {
            using RawData = RawData_;
            constexpr static std::size_t unit_size = unit_size_;

            // RawDataを格納できるのに十分な大きさのチャンク数(1チャンクは1byte * unit_size)
            static constexpr std::size_t chunks_size = sizeof(RawData) / unit_size_ + (sizeof(RawData) % unit_size_)? 1 : 0;
            // 最後のチャンクのうち有効なチャンクのバイト数
            static constexpr std::uint8_t last_size = (sizeof(RawData) % unit_size_) ? (sizeof(RawData) % unit_size_) : unit_size_;

            std::uint8_t chunks[chunks_size][unit_size_]{};

            Serialize(const low_cost_ref_val_t<RawData> raw_data)
            {
                std::memcpy(chunks, &raw_data, sizeof(RawData));
            }

            explicit operator RawData&() noexcept
            {
                return *static_cast<RawData *>(chunks);
            }
        };
    }
}