#pragma once

#include <cstddef>
#include <type_traits>
#include <utility>
#include <iterator>
#include <stdexcept>
#include <algorithm>
#include <execution>
#include <compare>

namespace StewLib
{
    template<class T, std::size_t n>
        requires std::is_object_v<T>
    struct Array final
    {
        using value_type = T;
        using pointer = T *;
        using const_pointer = const T *;
        using reference = T&;
        using const_reference = const T&;

        struct iterator
        {
            T * p;

            constexpr T * operator ->() const noexcept
            {
                return p;
            }

            friend constexpr iterator& operator ++(iterator& iter) noexcept
            {
                ++iter.p;
                return iter;
            }

            friend constexpr iterator operator ++(iterator& iter, int) noexcept
            {
                return {iter.p++};
            }

            friend constexpr iterator& operator --(iterator& iter) noexcept
            {
                --iter.p;
                return iter;
            }

            friend constexpr iterator operator --(iterator& iter, int) noexcept
            {
                return {iter.p--};
            }

            friend constexpr iterator operator +(const iterator& iter, std::ptrdiff_t diff) noexcept
            {
                return {iter.p + diff};
            }

            friend constexpr iterator operator +(std::ptrdiff_t diff, const iterator& iter) noexcept
            {
                return {diff + iter.p};
            }

            friend constexpr iterator& operator +=(iterator& iter, std::ptrdiff_t diff) noexcept
            {
                iter.p += diff;
                return iter;
            }

            friend constexpr iterator operator -(const iterator& iter, std::ptrdiff_t diff) noexcept
            {
                return {iter.p - diff};
            }

            friend constexpr iterator& operator -=(iterator& iter, std::ptrdiff_t diff) noexcept
            {
                iter.p -= diff;
                return iter;
            }

            friend constexpr std::ptrdiff_t operator -(const iterator& l, const iterator& r) noexcept
            {
                return l.p - r.p;
            }

            friend constexpr T& operator *(const iterator& iter) noexcept
            {
                return *iter.p;
            }

            constexpr T& operator [](std::size_t i) const noexcept
            {
                return *(p + i);
            }

            friend auto operator<=>(const iterator&, const iterator&) = default;

            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using iterator_category = std::contiguous_iterator_tag;
        };

        static_assert(std::contiguous_iterator<iterator>);

        struct const_iterator
        {
            const T * p;

            constexpr const T * operator ->() const noexcept
            {
                return p;
            }

            friend constexpr const_iterator& operator ++(const_iterator& iter) noexcept
            {
                ++iter.p;
                return iter;
            }

            friend constexpr const_iterator operator ++(const_iterator& iter, int) noexcept
            {
                return {iter.p++};
            }

            friend constexpr const_iterator& operator --(const_iterator& iter) noexcept
            {
                --iter.p;
                return iter;
            }

            friend constexpr const_iterator operator --(const_iterator& iter, int) noexcept
            {
                return {iter.p--};
            }

            friend constexpr const_iterator operator +(const const_iterator& iter, std::ptrdiff_t diff) noexcept
            {
                return {iter.p + diff};
            }

            friend constexpr const_iterator operator +(std::ptrdiff_t diff, const const_iterator& iter) noexcept
            {
                return {diff + iter.p};
            }

            friend constexpr const_iterator& operator +=(const_iterator& iter, std::ptrdiff_t diff) noexcept
            {
                iter.p += diff;
                return iter;
            }

            friend constexpr const_iterator operator -(const const_iterator& iter, std::ptrdiff_t diff) noexcept
            {
                return {iter.p - diff};
            }

            friend constexpr const_iterator& operator -=(const_iterator& iter, std::ptrdiff_t diff) noexcept
            {
                iter.p -= diff;
                return iter;
            }

            friend constexpr std::ptrdiff_t operator -(const const_iterator& l, const const_iterator& r) noexcept
            {
                return l.p - r.p;
            }

            friend constexpr const T& operator *(const const_iterator& iter) noexcept
            {
                return *iter.p;
            }

            constexpr const T& operator [](std::size_t i) const noexcept
            {
                return *(p + i);
            }

            friend auto operator<=>(const const_iterator&, const const_iterator&) = default;

            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using iterator_category = std::contiguous_iterator_tag;
        };

        static_assert(std::contiguous_iterator<const_iterator>);

        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        T elements[n]{};

        friend constexpr Array to_array(const T(&elements)[n]) noexcept(std::is_nothrow_copy_constructible_v<T>)
        {
            Array ret;
            for(std::size_t i = 0; i < n; ++i)
            {
                ret.elements[i] = elements[i];
            }
            return ret;
        }

        friend constexpr Array to_array(T(&&elements)[n]) noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            Array ret;
            for(std::size_t i = 0; i < n; ++i)
            {
                ret.elements[i] = std::move(elements[i]);
            }
            return ret;
        }

        constexpr T& at(std::size_t i) & noexcept(false)
        {
            if(n <= i)
            {
                throw std::out_of_range{"StewLib: Out Of Range."};
            }
            else return elements[i];
        }
        
        constexpr const T& at(std::size_t i) const & noexcept(false)
        {
            if(n <= i)
            {
                throw std::out_of_range{"StewLib: Out Of Range."};
            }
            else return elements[i];
        }
        
        constexpr T&& at(std::size_t i) && noexcept(false)
        {
            if(n <= i)
            {
                throw std::out_of_range{"StewLib: Out Of Range."};
            }
            else return elements[i];
        }
        
        constexpr const T&& at(std::size_t i) const && noexcept(false)
        {
            if(n <= i)
            {
                throw std::out_of_range{"StewLib: Out Of Range."};
            }
            else return elements[i];
        }

        constexpr T& operator [](std::size_t i) & noexcept
        {
            return elements[i];
        }
        
        constexpr const T& operator [](std::size_t i) const & noexcept
        {
            return elements[i];
        }
        
        constexpr T&& operator [](std::size_t i) && noexcept
        {
            return elements[i];
        }
        
        constexpr const T&& operator [](std::size_t i) const && noexcept
        {
            return elements[i];
        }

        constexpr T& front() & noexcept
        {
            return elements[0];
        }
        
        constexpr const T& front() const & noexcept
        {
            return elements[0];
        }
        
        constexpr T&& front() && noexcept
        {
            return elements[0];
        }
        
        constexpr const T&& front() const && noexcept
        {
            return elements[0];
        }

        constexpr T& back() & noexcept
        {
            return elements[n - 1];
        }
        
        constexpr const T& back() const & noexcept
        {
            return elements[n - 1];
        }
        
        constexpr T&& back() && noexcept
        {
            return elements[n - 1];
        }
        
        constexpr const T&& back() const && noexcept
        {
            return elements[n - 1];
        }
        
        constexpr T * data() noexcept
        {
            return {elements};
        }

        constexpr const T * data() const noexcept
        {
            return {elements};
        }

        constexpr iterator begin() noexcept
        {
            return {elements};
        }
        
        constexpr const_iterator begin() const noexcept
        {
            return {elements};
        }

        constexpr iterator end() noexcept
        {
            return {elements + n};
        }
        
        constexpr const_iterator end() const noexcept
        {
            return {elements + n};
        }

        constexpr const_iterator cbegin() const noexcept
        {
            return {elements};
        }

        constexpr const_iterator cend() const noexcept
        {
            return {elements + n};
        }

        constexpr reverse_iterator rbegin() noexcept
        {
            return {elements + n};
        }
        
        constexpr const_reverse_iterator rbegin() const noexcept
        {
            return {elements + n};
        }

        constexpr reverse_iterator rend() noexcept
        {
            return {elements};
        }
        
        constexpr const_reverse_iterator rend() const noexcept
        {
            return {elements};
        }

        constexpr const_reverse_iterator crbegin() const noexcept
        {
            return {elements + n};
        }

        constexpr const_reverse_iterator crend() const noexcept
        {
            return {elements};
        }

        [[nodiscard]] constexpr bool empty() const noexcept
        {
            return n;
        }

        constexpr std::size_t size() const noexcept
        {
            return n;
        }

        constexpr std::size_t max_size() const noexcept
        {
            return n;
        }

        constexpr void fill(const T& u) noexcept(noexcept(std::fill_n(begin(), n, u)))
        {
            std::fill_n(begin(), n, u);
        }

        template<class ExecutionPolicy>
            requires std::is_execution_policy_v<ExecutionPolicy>
        constexpr void fill(const T& u, ExecutionPolicy&& exec) noexcept(noexcept(std::fill_n(exec, begin(), n, u)))
        {
            std::fill_n(exec, begin(), n, u);
        }

        constexpr void swap(Array& other) noexcept(noexcept(std::swap(std::declval<T&>(), std::declval<T&>())))
        {
            std::swap_ranges(begin(), end(), other.begin());
        }

        auto operator <=>(const Array&) const = default;

        friend constexpr void swap(Array& l, Array& r) noexcept(noexcept(l.swap(r)))
        {
            l.swap(r);
        }

        template<std::size_t i>
        friend constexpr T& get(Array& x) noexcept
        {
            static_assert(i < x.size(), "StewLib: Out Of Range.");
            return x.elements[i];
        }
        
        template<std::size_t i>
        friend constexpr const T& get(const Array& x) noexcept
        {
            static_assert(i < x.size(), "StewLib: Out Of Range.");
            return x.elements[i];
        }
        
        template<std::size_t i>
        friend constexpr T&& get(Array&& x) noexcept
        {
            static_assert(i < x.size(), "StewLib: Out Of Range.");
            return x.elements[i];
        }
        
        template<std::size_t i>
        friend constexpr const T&& get(const Array&& x) noexcept
        {
            static_assert(i < x.size(), "StewLib: Out Of Range.");
            return x.elements[i];
        }
    };

    template<class T, class ... U>
    Array(T, U...) -> Array<T, 1 + sizeof...(U)>;
}

namespace std
{
    template <class> class tuple_size;

    template <class T, std::size_t n>
    struct tuple_size<StewLib::Array<T, n>> : integral_constant<std::size_t, n>
    {};

    template <std::size_t i, class T> class tuple_element;

    template <std::size_t i, class T, std::size_t n>
    struct tuple_element<i, StewLib::Array<T, n>>
    {
        static_assert(i < n, "StewLib: Out Of Range.");
        using type = T;
    };
}