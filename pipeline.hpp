// 使いどころあるか...？
// ラムダ式でよくない？

#pragma once

#include <cstdint>
#include <tuple>
#include <utility>

namespace StewLib
{
    namespace Implement::Pipeline
    {
        template<class Func>
        struct FuncAdapter final
        {
            Func func;

            consteval FuncAdapter(const Func& func):
                func{func}
            {}

            consteval FuncAdapter(const FuncAdapter&) = default;
            consteval FuncAdapter(FuncAdapter&&) = default;

            template<class ... Args>
            decltype(auto) operator ,(Args&& ... args)
            {
                return func(std::forward<Args>(args) ...);
            }
        };
    }

    template<class ... Func>
    class Pipeline final
    {
        std::tuple<Implement::Pipeline::FuncAdapter<Func> ...> funcs;

    public:
        consteval Pipeline(const Implement::Pipeline::FuncAdapter<Func>& ... funcs):
            funcs{funcs ...}
        {}

    private:
        template<std::size_t ... indexs>
        consteval Pipeline(const std::tuple<Func...>& funcs, const std::index_sequence<indexs ...>&):
            funcs{std::get<indexs>(funcs) ...}
        {}

    public:
        template<class ... Args>
        decltype(auto) call(Args&& ... args)
        {
            return [this, ... args2 = std::forward<Args>(args)]<std::size_t ... indexs>(const std::index_sequence<indexs ...>&)
                {
                    return (std::forward_as_tuple<Args ...>(args2 ...) , ... , std::get<indexs>(funcs));
                }(std::make_index_sequence<sizeof...(Func)>());
        }

        template<class ... FuncL, class ... FuncR>
        friend inline consteval auto operator <<(const Pipeline<FuncL ...>& l, const Pipeline<FuncR ...>& r)
        {
            return Pipeline{std::tuple_cat(l.funcs, r.funcs), std::make_index_sequence<sizeof...(FuncL) + sizeof...(FuncR)>()};
        }
    };
}