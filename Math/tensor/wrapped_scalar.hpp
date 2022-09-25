#pragma once

#include <type_traits>

namespace StewLib
{
    template<class T>
        requires std::is_object_v<T>
    struct WrappedScalar final
    {
        T value;

        WrappedScalar() = default;
        WrappedScalar(const WrappedScalar&) = default;
        WrappedScalar(WrappedScalar&&) = default;
        WrappedScalar& operator =(const WrappedScalar&) = default;
        WrappedScalar& operator =(WrappedScalar&&) = default;
        ~WrappedScalar() = default;

        template<class T2>
            requires std::is_object_v<T2>
        WrappedScalar(const T2 value) noexcept:
            value{static_cast<T>(value)}
        {}

        template<class T2>
        WrappedScalar(const WrappedScalar<T2>& obj) noexcept:
            value{static_cast<T>(obj.value)}
        {}

        constexpr operator T&() & noexcept
        {
            return value;
        }

        constexpr operator const T&() const & noexcept
        {
            return value;
        }

        constexpr operator T&&() && noexcept
        {
            return std::move(value);
        }

        constexpr operator const T&&() const && noexcept
        {
            return std::move(value);
        }
    };
}