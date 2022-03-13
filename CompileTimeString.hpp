#pragma once

// C++14想定
#include <cstdint>
#include <type_traits>
// debug
#include <iostream>
#include <cmath>

#include "Rank.hpp"
#include "Enumerate.hpp"

namespace StewStringCpp14
{
    template<size_t N>
    struct String;

    template<size_t N>
    constexpr String<N> makeString(const char (& obj)[N]) noexcept;
    template<size_t N>
    constexpr String<N> makeString(const char *const str) noexcept;
    constexpr String<16> makeString(const float&& x) noexcept;

    template<size_t N>
    struct String
    {
        template<size_t N2>
        friend constexpr String<N2> makeString(const char (& obj)[N2]) noexcept;
        template<size_t N2>
        friend constexpr String<N2> makeString(const char *const str) noexcept;
        friend constexpr String<16> makeString(const float&& x) noexcept;

        char buffer[N]{};
        static constexpr const size_t capacity = N;

        constexpr String() noexcept = default;
        String(const String&) noexcept = default;
        String& operator=(const String&) noexcept = default;
        ~String() noexcept = default;

        /* 委譲コンパイラでNを推論させたかったができなかった。constexprだと推論補助できないのも地味につらい。
        template<class T>
        constexpr String(T&& obj) noexcept:
            String<N>(obj,StewRank::Rank<1>())
        {}
        */

        private:
        constexpr String(const char (& arr_str)[N],const StewRank::Rank<1>&) noexcept:
            String(arr_str,typename StewEnumerate::EnumerateMake<N>::type())
        {}

        constexpr String(const char *const str,const StewRank::Rank<0>&) noexcept:
            String(str,typename StewEnumerate::EnumerateMake<N>::type())
        {}

        template<size_t ... Numbers> // デフォルト引数で型の推論できないのなんで...？
        constexpr String(const char *const str,StewEnumerate::Enumerate<Numbers ...>) noexcept:
            buffer{str[Numbers]...}
        {}

        public:
        constexpr String(const char c) noexcept:
            buffer{c}
        {}

        template<size_t N2>
        constexpr String(const String<N2>& obj) noexcept:
            String(obj.buffer,typename StewEnumerate::EnumerateMake<N2>::type())
        {}

        template<size_t N2>
        constexpr String& operator=(const String<N2>& obj) noexcept
        {
            const size_t size = obj.size();
            const char *const obj_buffer = obj.buffer;
            char *const this_buffer = buffer;
            for(size_t i = 0;i<size;++i)
            {
                this_buffer[i] = obj_buffer[i];
            }
        }

        constexpr char operator[](const size_t i) const noexcept
        {
            return buffer[i];
        }

        constexpr operator const char *() const noexcept
        {
            return buffer;
        }

        constexpr size_t size() const noexcept
        {
            size_t i = 0;
            const char *const this_buffer = buffer;
            for(;this_buffer[i];++i);
            return i;
        }

        constexpr size_t length() const noexcept
        {
            size_t i = 0;
            const char *const this_buffer = buffer;
            for(;this_buffer[i];++i);
            return i? i-1 : 0;
        }

        constexpr String& operator+=(const char *const str) noexcept
        {
            const size_t this_size = size();
            char *const this_buffer = buffer;
            size_t i = 0;
            for(;str[i];++i) this_buffer[this_size + i] = str[i];
            this_buffer[this_size + i] = '\0';

            return *this;
        }

        constexpr String& operator+=(const char c) noexcept
        {
            const size_t this_size = size();
            buffer[this_size] = c;
            buffer[this_size + 1] = '\0';

            return *this;
        }
    };

    template<size_t N>
    constexpr String<N> makeString(const char (& obj)[N]) noexcept
    {
        return {obj,StewRank::Rank<1>()};
    }

    template<size_t N>
    constexpr String<N> makeString(const char *const str) noexcept
    {
        return {str,StewRank::Rank<0>()};
    }

    /* 十進数のクラスができたらやる。
    template<size_t N>
    constexpr String<N> makeString(const float&& x) noexcept
    {

        auto ret = makeString("null");
        return ret;
    }*/

    template<size_t Lsize,size_t Rsize>
    constexpr String<Lsize + Rsize - 1> operator+(const String<Lsize>& l,const String<Rsize>& r) noexcept
    {
        String<Lsize + Rsize - 1> ret = l;
        ret += r;
        return ret;
    }
}
