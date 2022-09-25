#pragma once

#include "../static_warn.hpp"
Stew_static_warn(true, "This File uses GNU extension (constexpr mathmatic functions).");
#include "../noexcept_auto.hpp"

#include <cmath>

namespace StewLib::Math
{
    constexpr auto cos(const auto& x) Stew_noexcept_auto
    {
        return std::cos(x);
    }

    constexpr auto sin(const auto& x) Stew_noexcept_auto
    {
        return std::sin(x);
    }

    constexpr auto atan2(const auto& y, const auto& x) Stew_noexcept_auto
    {
        return std::atan2(y, x);
    }

    constexpr auto sqrt(const auto& x) Stew_noexcept_auto
    {
        return std::sqrt(x);
    }
}