/*
どうもよっぽど大きな計算をするでもなきゃ並列化の恩恵はほぼないっぽい？
par_unseq指定がseq指定より遅くなることがないと信じたい。
*/

#pragma once

#include <concepts>
#include <utility>
#include <array>
#include <vector>
#include <execution>
#include <algorithm>

namespace StewLib
{
    template<typename T>
    constexpr inline std::vector<T> index_vec(const T n) noexcept
    {
        std::vector<T> ret(n);
        for(T i = 0; i < n; ++i)
        {
            ret[i] = i;
        }
        return ret;
    }

    namespace ParallelImplement
    {
        template<std::integral T, T ... indexs>
        std::array<T, sizeof...(indexs)> index_arr_inner(const std::integer_sequence<T, indexs ...>&) noexcept
        {
            return {indexs ...};
        }
    }

    template<std::integral T, T n>
    constexpr std::array<decltype(n), n> index_arr() noexcept
    {
        return ParallelImplement::index_arr_inner(std::make_integer_sequence<decltype(n), n>());
    }

    template<class ExecutionPolicy, typename T, typename F>
    constexpr inline void for_i(ExecutionPolicy&& exec, T end, F for_body) noexcept
    {
        const auto indexs = index_vec(end);
        std::for_each(exec, indexs.cbegin(), indexs.cend(), for_body);
    }

    template<class ExecutionPolicy, auto n, class F>
    constexpr inline void for_i(F for_body) noexcept
    {
        // constexpr auto indexs = index_arr<decltype(n), n>();
        const auto indexs = index_arr<decltype(n), n>();
        constexpr auto exec = ExecutionPolicy();
        std::for_each(exec, indexs.cbegin(), indexs.cend(), for_body);
    }
}