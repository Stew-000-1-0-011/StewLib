#error "This code is completely wrong!"

#if 0
/*
operator.()はオーバーロードできない！
*/

#pragma once

#include <memory>

#include "judge_lower_cost_than_ref.hpp"

namespace StewLib
{
    namespace
    {
        // コピー、ムーブ構築・代入が可能な参照型
        template<class T>
        struct Ref final
        {
        private:
            T *const p{nullptr};

        public:
            constexpr Ref(T& obj) noexcept:
                p{std::addressof(obj)}
            {}

            Ref() = delete;
            Ref(const Ref&) = default;
            Ref(Ref&&) = default;
            ~Ref() = default;

            // T& operator=(const Ref& ref) noexcept
            // {
            //     if(std::addressof(ref) == this)
            //     {
            //         return *p;
            //     }

            //     p = ref.p;

            //     return *p;
            // }

            // T& operator=(Ref&& ref) noexcept
            // {
            //     if(std::addressof(ref) == this)
            //     {
            //         return *p;
            //     }

            //     p = ref.p;

            //     return *p;
            // }

            // 演算子でない関数もこうするつもりか？キリがない。
            T& operator=(const low_cost_ref_val_t<T> obj)
            {
                *p = obj;
                return *p;
            }

            // T& operator+=(const low_cost_ref_val_t<T> obj)
            // {
            //     *p += obj;
            //     return *p;
            // }

            // T& operator-=(const low_cost_ref_val_t<T> obj)
            // {
            //     *p -= obj;
            //     return *p;
            // }

            // T& operator*=(const low_cost_ref_val_t<T> obj)
            // {
            //     *p *= obj;
            //     return *p;
            // }

            // T& operator%=(const low_cost_ref_val_t<T> obj)
            // {
            //     *p %= obj;
            //     return *p;
            // }

            // T& operator/=(const low_cost_ref_val_t<T> obj)
            // {
            //     *p /= obj;
            //     return *p;
            // }

            // T& operator<<=(const low_cost_ref_val_t<T> obj)
            // {
            //     *p <<= obj;
            //     return *p;
            // }

            // T& operator>>=(const low_cost_ref_val_t<T> obj)
            // {
            //     *p >>= obj;
            //     return *p;
            // }

            // T& operator&=(const low_cost_ref_val_t<T> obj)
            // {
            //     *p &= obj;
            //     return *p;
            // }

            // T& operator|=(const low_cost_ref_val_t<T> obj)
            // {
            //     *p |= obj;
            //     return *p;
            // }

            // T& operator^=(const low_cost_ref_val_t<T> obj)
            // {
            //     *p ^= obj;
            //     return *p;
            // }

            // T operator++(int)
            // {
            //     (*p)++;
            //     return *p;
            // }

            // T operator--(int)
            // {
            //     (*p)--;
            //     return *p;
            // }

            // T& operator++()
            // {
            //     ++(*p);
            //     return *p;
            // }

            // T& operator--()
            // {
            //     --(*p);
            //     return *p;
            // }

            T* operator&() const noexcept
            {
                return p;
            }

            // operator T() const noexcept
            // {
            //     return *p;
            // }

            operator T&() const noexcept
            {
                return *p;
            }
        };
    }
}

#endif