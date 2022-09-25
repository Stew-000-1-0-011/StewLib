// #pragma once

// #include <type_traits>
// #include <utility>
// #include <compare>
// #include <memory>

// namespace StewLib
// {
//     template<class T, class Alloc = std::allocator<T>>
//         requires std::is_object_v<T>
//     struct Uninitialized final
//     {
//         Alloc a;
//         T * p;

//         constexpr Uninitialized(const Alloc& a = Alloc{}) noexcept:
//             a{a},
//             p{a.allocate(1)}
//         {};

//         constexpr Uninitialized(const Uninitialized& obj, const Alloc& a = Alloc{}) noexcept(std::is_nothrow_copy_constructible_v<T>):
//             a{a},
//             p{a.allocate(1)}
//         {}

//         constexpr Uninitialized(Uninitialized&& obj) noexcept(std::is_nothrow_move_constructible_v<T>):
//             value{std::move(obj.value)}
//         {}

//         constexpr Uninitialized& operator =(const Uninitialized& obj) noexcept(std::is_nothrow_copy_assignable_v<T>)
//         {
//             value = obj.value;
//             return *this;
//         }

//         constexpr Uninitialized& operator =(Uninitialized&& obj) noexcept(std::is_nothrow_move_assignable_v<T>)
//         {
//             value = std::move(obj.value);
//             return *this;
//         }

//         constexpr ~Uninitialized()
//         {}

//         constexpr Uninitialized(const T& value) noexcept(std::is_copy_constructible_v<T>):
//             value{value}
//         {}

//         constexpr Uninitialized(T&& value) noexcept(std::is_move_constructible_v<T>):
//             value{std::move(value)}
//         {}

//         constexpr init(auto&& ... args) noexcept()
//         {
//             std::
//         }

//         // これらってUB？
//         operator T&() & noexcept
//         {
//             return value;
//         }

//         operator const T&() const& noexcept
//         {
//             return value;
//         }

//         operator T&&() && noexcept
//         {
//             return value;
//         }

//         operator const T&&() const&& noexcept
//         {
//             return value;
//         }

//         friend constexpr auto operator <=>(const Uninitialized& l, const Uninitialized& r) noexcept(noexcept(l.value <=> r.value))
//         {
//             return l.value <=> r.value;
//         }
//     };

//     struct uninitialized_tag_t{} uninitialized_tag;
// }