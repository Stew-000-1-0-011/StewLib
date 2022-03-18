#pragma once

#include <type_traits>

/// TODO: ビット環境を指定
#define Stew_64bit_env

#ifndef Stew_64bit_env
#ifndef Stew_32bit_env

#error "You must determine 32 or 64 env."

#endif
#endif

namespace StewLib
{
    namespace
    {
        #ifdef Stew_64bit_env
        template<class T>
        concept is_smaller_than_ref = sizeof(T) <= 8;
        #endif

        #ifdef Stew_32bit_env
        template<class T>
        concept is_smaller_than_ref = sizeof(T) <= 4;
        #endif

        template<class T>
        concept is_lower_cost_than_ref =
        is_smaller_than_ref<T> && std::is_trivially_copy_constructible_v<T> &&
        std::is_trivially_move_constructible_v<T> && std::is_trivially_destructible_v<T>;

        template<class T>
        struct low_cost_ref_val final
        {
            static_assert([]{return false;}(), "T must not be cvref.");
        };

        template<class T>
        requires std::same_as<T, std::remove_cvref_t<T>>
        struct low_cost_ref_val<T> final
        {
            using type = T&;
        };

        template<class T>
        requires std::same_as<T, std::remove_cvref_t<T>> && is_lower_cost_than_ref<T>
        struct low_cost_ref_val<T> final
        {
            using type = T;
        };

        template<class T>
        using low_cost_ref_val_t = typename low_cost_ref_val<T>::type;
    }
}