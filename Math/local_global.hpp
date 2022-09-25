#pragma once

#include <concepts>

namespace StewLib::Math
{
    struct Local{};
    struct Global{};

    template<class T>
    concept IsL = std::same_as<T, Local>;

    template<class T>
    concept IsG = std::same_as<T, Global>;

    template<class T>
    concept IsLG = IsL<T> || IsG<T>;
}