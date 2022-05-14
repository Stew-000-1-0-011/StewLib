#pragma once

#include <type_traits>
#ifdef __cpp_lib_concepts
#include <concepts>
#endif

namespace StewLib
{
    template<typename T, T value_>
    #ifdef __cpp_concepts
    requires std::integral<T> && std::same_as<T, std::remove_cvref_t<T>>
    #endif
    struct IntegerHolder final
    {
        static constexpr T value = value_;
    };
}