#pragma once
#include <type_traits>

namespace StewLib
{

    template<class T>
#if __cplusplus > 202002L
    [[deprecated("StewLib: Use std::to_underlying instead of this.")]] 
#endif
    constexpr std::underlying_type_t<T> to_underlying(T value) noexcept
    {
        return static_cast<std::underlying_type_t<T>>(value);
    }
}