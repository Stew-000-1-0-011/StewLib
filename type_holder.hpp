#pragma once

#include <type_traits>
#include <concepts>
#include <cstddef>

#include "utility.hpp"

namespace StewLib
{
    template<class ... Ts>
    struct TypeHolder;

    namespace Implement::TypeHolderImp
    {
        enum SizeT : std::size_t;

        consteval SizeT operator ^(const SizeT count, const bool pred_result) noexcept
        {
            if (count == 0)
            {
                return static_cast<SizeT>(pred_result);
            }
            else
            {
                return static_cast<SizeT>(count + static_cast<std::size_t>(1));
            }
        }

        template<std::size_t purpose_reversed_index, class Head, class ... Ts>
        struct GetTypeHelper
        {
            using Type = typename GetTypeHelper<purpose_reversed_index, Ts...>::Type;
        };

        template<std::size_t purpose_reversed_index, class Head, class ... Ts>
            requires (sizeof...(Ts) == purpose_reversed_index)
        struct GetTypeHelper<purpose_reversed_index, Head, Ts ...>
        {
            using Type = Head;
        };

        template<std::size_t purpose_reversed_index, class Head>
        struct GetTypeHelper<purpose_reversed_index, Head>
        {
            using Type = Head;
        };

    }

    template<class ... Ts>
    struct TypeHolder final
    {
        template</*IsPred*/ auto pred> //MSVCで何故かエラー。
            requires StewLib::is_only_one<pred, Ts ...>
        static consteval std::size_t get_index() noexcept
        {
            return sizeof...(Ts) - (static_cast<Implement::TypeHolderImp::SizeT>(0) ^ ... ^ pred.template operator() < Ts > ());
        }

        template<std::size_t i, IsPred auto pred>
        struct GetType;

        template<std::size_t i>
            requires (i <= sizeof...(Ts) - 1)
        struct GetType<i, empty_pred> final
        {
            using Type = typename Implement::TypeHolderImp::GetTypeHelper<sizeof...(Ts) - 1 - i, Ts ...>::Type;
        };

        template<std::size_t i, IsPred auto pred>
            requires (!std::same_as<decltype(pred), decltype(empty_pred)>&& StewLib::is_only_one<pred, Ts ...>)
        struct GetType<i, pred> final
        {
            using Type = typename GetType<get_index<pred>(), empty_pred>::Type;
        };

        template<std::size_t i = 0, IsPred auto pred = empty_pred>
        using GetTypeT = typename GetType<i, pred>::Type;

        template<IsPred auto pred>
        static constexpr bool is_only_one = StewLib::is_only_one<pred, Ts ...>;

        template<IsPred auto pred>
        static constexpr bool is_all = StewLib::is_all<pred, Ts ...>;

        static constexpr std::size_t size = sizeof...(Ts);

        static constexpr bool is_non_duplicated = (StewLib::is_only_one<is_same_lambda<Ts>, Ts ...> && ...);
    };

    template<>
    struct TypeHolder<> final
    {
        template<IsPred auto pred>
        static consteval std::size_t get_index() noexcept = delete;

        template<std::size_t i, IsPred auto pred>
        struct GetType
        {
            static_assert((i, false));
        };

        template<std::size_t i>
        struct GetType<i, empty_pred> final
        {
            static_assert((i, false));
        };

        template<IsPred auto pred>
        struct GetType<0, pred> final
        {
            static_assert((pred, false));
        };

        template<std::size_t i = 0, IsPred auto pred = empty_pred>
        using GetTypeT = typename GetType<i, pred>::Type;

        template<IsPred auto pred>
        static constexpr bool is_only_one = [] {static_assert([] {return false; }()); return false; }();

        template<IsPred auto pred>
        static constexpr bool is_all = [] {static_assert([] {return false; }()); return false; }();

        static constexpr std::size_t size = 0;

        static constexpr bool is_non_duplicated = true;
    };
}