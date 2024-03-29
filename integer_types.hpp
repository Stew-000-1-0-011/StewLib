#pragma once
#include <cstddef>
#include <cstdint>

namespace StewLib::IntegerTypes
{
    using i8 = std::int8_t;
    using u8 = std::uint8_t;
    using i16 = std::int16_t;
    using u16 = std::uint16_t;
    using i32 = std::int32_t;
    using u32 = std::uint32_t;
    using i64 = std::int64_t;
    using u64 = std::uint64_t;
    using i_ptr = std::intptr_t;
    using ptrdiff_t = std::ptrdiff_t;
    using size_t = std::size_t;
    using nullptr_t = std::nullptr_t;
    using max_align_t = std::max_align_t;
    using byte = std::byte;
}
