// boostに慣れたらboostのcircular_bufferを使うべきかも。

/*
バッファの動的拡張をサポートしたりしたい。
多分バッファの終端を直に.end()でとるのではなく、現在のリングバッファの終端を保持しておき、
その終端をpushなりpopなりで通り過ぎたら現在の終端に更新する...とかで上手くいきそう。
*/

#include <utility>
#include <type_traits>
#include <initializer_list>
#include <memory>
#include <optional>

namespace StewLib
{
    template<template<class ...> class Container, class T, class Allocator = std::allocator<T>>
    class CircularBuffer final
    {
    public:
        using BufferType = Container<T, Allocator>;

    private:
        BufferType buffer;
        BufferType::iterator iter{buffer.begin()};
        std::size_t count{};

    public:
        template<class ... Args>
        constexpr CircularBuffer(Args&& ... args) noexcept:
            buffer(args ...)
        {}

        CircularBuffer(const CircularBuffer&) = default;
        CircularBuffer(CircularBuffer&&) = default;
        CircularBuffer& operator=(const CircularBuffer&) = default;
        CircularBuffer& operator=(CircularBuffer&&) = default;

        template<class U>
        void push(U&& obj) noexcept
        {
            if(const auto size = buffer.size(); ++count > size)
            {
                count = size;
            }

            *iter = std::forward<U>(obj);
            if(++iter == buffer.end())
            {
                iter = buffer.begin();
            }
        }

        T pop() noexcept
        {
            --count;

            if(--iter == buffer.begin() - 1)
            {
                iter = buffer.end() - 1;
            }

            return std::move(*iter);
        }

        void clear() noexcept
        {
            count = 0;
        }
        
        BufferType& get_buffer() noexcept
        {
            return buffer;
        }
    };

    template<template<class ...> class Container, class T, class Allocator = std::allocator<T>>
    class SafeCircularBuffer final
    {
    public:
        using BufferType = Container<T, Allocator>;

    private:
        BufferType buffer;
        BufferType::iterator iter{buffer.begin()};
        std::size_t count{};

    public:
        template<class ... Args>
        constexpr SafeCircularBuffer(Args&& ... args) noexcept:
            buffer(args ...)
        {}

        SafeCircularBuffer(const SafeCircularBuffer&) = default;
        SafeCircularBuffer(SafeCircularBuffer&&) = default;
        SafeCircularBuffer& operator=(const SafeCircularBuffer&) = default;
        SafeCircularBuffer& operator=(SafeCircularBuffer&&) = default;

        template<class U>
        void push(U&& obj) noexcept
        {
            
            if(const auto size = buffer.size(); ++count > size)
            {
                count = size;
            }

            *iter = std::forward<U>(obj);
            if(++iter == buffer.end())
            {
                iter = buffer.begin();
            }
        }

        std::optional<T> pop() noexcept
        {
            if(!count)
            {
                return std::nullopt;
            }

            --count;

            if(--iter == buffer.begin() - 1)
            {
                iter = buffer.end() - 1;
            }

            return std::move(*iter);
        }
        
        BufferType& get_buffer() noexcept
        {
            return buffer;
        }

        void clear() noexcept
        {
            count = 0;
        }
    };
}