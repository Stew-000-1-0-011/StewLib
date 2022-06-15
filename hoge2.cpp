#include <iostream>
#include "named_param.hpp"

constexpr auto pred = []<class T>() constexpr noexcept{return std::same_as<std::remove_cvref_t<T>, bool>;};

namespace hoge_param
{
    stew_define_named_parameter(a, int);
    stew_define_named_parameter(b, int&);
    stew_define_named_parameter(c, int &&);
    stew_define_named_parameter(d, const int&);
    stew_define_named_parameter(e, StewLib::ForwardRef);
    stew_define_named_parameter(f, StewLib::ForwardRef, pred);
}

void fuga(auto&& ... args)
{
    auto&& param_list = StewLib::make_named_params<StewLib::TypeHolder<hoge_param::a_tag, hoge_param::b_tag>>(std::forward<decltype(args)>(args) ...);
    // auto&& param_list = StewLib::make_named_params<StewLib::TypeHolder<hoge_param::a_tag, hoge_param::b_tag>>();
    int a;
    if constexpr(std::remove_cvref_t<decltype(param_list)>::is_in(hoge_param::a))
    {
        a = 1;
    }
    else a = -100;

    std::cout <<"fuga: " << a << std::endl;
}

int main()
{
    int b = 77;
    // fuga(hoge_param::c = 0);
    fuga(hoge_param::a = 88);
    fuga(hoge_param::b = b);
}