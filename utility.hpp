#pragma once
#include <type_traits>
#include <concepts>

namespace StewLib
{
    // 任意の型について、が書けないものか。
    template<class F>
    concept IsPred = true;
    // concept IsPred = requires(F pred){{pred.template operator()<AllType>()} -> std::same_as<bool>;};

    template<IsPred auto pred, class ... Ts>
    inline constexpr bool is_only_one = (pred.template operator()<Ts>() + ...) == 1;

    template<IsPred auto pred, class ... Ts>
    inline constexpr bool is_all = (pred.template operator()<Ts>() && ...);

    inline constexpr auto empty_pred = []<class T>(){return true;};

    inline constexpr auto empty_lambda = []<class T>(){};

    template<class T>
    inline constexpr auto is_same_lambda = []<class U>() consteval noexcept {return std::same_as<T, U>;};

    template<class T>
#if __cplusplus > 202002L
    [[deprecated("StewLib: Use std::to_underlying instead of this.")]] 
#endif
    constexpr std::underlying_type_t<T> to_underlying(T value) noexcept
    {
        return static_cast<std::underlying_type_t<T>>(value);
    }
}