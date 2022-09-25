#pragma once

// GNU独自拡張前提(cmath関数のconstexpr化はよ)
// ↑errnoあたりの仕様のせいで難しいらしい(定数式評価か否かで分岐させればいいのでは？)

// メタクラスが使えるようになったら多分もっと楽に記述できそう。

// 型に対する制約を何かしら付けるべきなんだろうなぁ...

// 本物の数値計算ライブラリには遠く及ばない。そこらへんが使えるようになったら置き換えるかも。

#include <cstddef>
#include <cmath>
#include <utility>
#include <ostream>
#include <span>

#include "../array.hpp"
#include "../noexcept_auto.hpp"
#include "functions.hpp"
#include "ref_tensor.hpp"

namespace StewLib::Math
{
    template<class Elem, std::size_t ... orders>
    struct Tensor final
    {
        static constexpr Array<std::size_t, sizeof...(orders)> orders_array{orders ...};
        Array<Elem, (orders * ...)> elements{};

    public:
        constexpr auto operator ()(const std::integral auto ... indexs) & Stew_noexcept_auto
        requires (0 < sizeof...(indexs) && sizeof...(indexs) < sizeof...(orders))
        {
            return [this]<std::size_t ... indexs_rest>(std::index_sequence<indexs_rest ...>, const auto ... indexs)
            {
                return Implement::TensorImp::RefTensor<false, Elem, orders_array[indexs_rest] ...>{std::span<Elem, (orders_array[indexs_rest] * ...)>{elements.begin() + Implement::TensorImp::calc_index<orders ...>(indexs ...), (orders_array[indexs_rest] * ...)}};
            }(std::make_index_sequence<sizeof...(orders) - sizeof...(indexs)>(), indexs ...);
        }

        constexpr auto operator ()(const std::integral auto ... indexs) const& Stew_noexcept_auto
        requires (0 < sizeof...(indexs) && sizeof...(indexs) < sizeof...(orders))
        {
            return [this]<std::size_t ... indexs_rest>(std::index_sequence<indexs_rest ...>, const auto ... indexs)
            {
                return Implement::TensorImp::RefTensor<false, const Elem, orders_array[indexs_rest] ...>{std::span<const Elem, (orders_array[indexs_rest] * ...)>{elements.cbegin() + Implement::TensorImp::calc_index<orders ...>(indexs ...), (orders_array[indexs_rest] * ...)}};
            }(std::make_index_sequence<sizeof...(orders) - sizeof...(indexs)>(), indexs ...);
        }

        constexpr auto operator ()(const std::integral auto ... indexs) && Stew_noexcept_auto
        requires (0 < sizeof...(indexs) && sizeof...(indexs) < sizeof...(orders))
        {
            return [this]<std::size_t ... indexs_rest>(std::index_sequence<indexs_rest ...>, const auto ... indexs)
            {
                return Implement::TensorImp::RefTensor<true, Elem, orders_array[indexs_rest] ...>{std::span<Elem, (orders_array[indexs_rest] * ...)>{elements.begin() + Implement::TensorImp::calc_index<orders ...>(indexs ...), (orders_array[indexs_rest] * ...)}};
            }(std::make_index_sequence<sizeof...(orders) - sizeof...(indexs)>(), indexs ...);
        }

        constexpr auto operator ()(const std::integral auto ... indexs) const&& Stew_noexcept_auto
        requires (0 < sizeof...(indexs) && sizeof...(indexs) < sizeof...(orders))
        {
            return [this]<std::size_t ... indexs_rest>(std::index_sequence<indexs_rest ...>, const auto ... indexs)
            {
                return Implement::TensorImp::RefTensor<true, const Elem, orders_array[indexs_rest] ...>{std::span<const Elem, (orders_array[indexs_rest] * ...)>{elements.cbegin() + Implement::TensorImp::calc_index<orders ...>(indexs ...), (orders_array[indexs_rest] * ...)}};
            }(std::make_index_sequence<sizeof...(orders) - sizeof...(indexs)>(), indexs ...);
        }

        constexpr Elem& operator ()(const std::integral auto ... indexs) & Stew_noexcept_auto
        requires (sizeof...(indexs) == sizeof...(orders))
        {
            return elements[Implement::TensorImp::calc_index<orders ...>(indexs ...)];
        }
        
        constexpr const Elem& operator ()(const std::integral auto ... indexs) const & Stew_noexcept_auto
        requires (sizeof...(indexs) == sizeof...(orders))
        {
            return elements[Implement::TensorImp::calc_index<orders ...>(indexs ...)];
        }
        
        constexpr Elem&& operator ()(const std::integral auto ... indexs) && Stew_noexcept_auto
        requires (sizeof...(indexs) == sizeof...(orders))
        {
            return static_cast<Elem&&>(elements[Implement::TensorImp::calc_index<orders ...>(indexs ...)]);
        }
        
        constexpr const Elem&& operator ()(const std::integral auto ... indexs) const && Stew_noexcept_auto
        requires (sizeof...(indexs) == sizeof...(orders))
        {
            return static_cast<Elem&&>(elements[Implement::TensorImp::calc_index<orders ...>(indexs ...)]);
        }

        template<class T2>
        constexpr Tensor& operator =(const Tensor<T2, orders ...>& obj) Stew_noexcept_auto
        {
            elements = obj.elements;
            return *this;
        }

        template<class T2>
        constexpr Tensor& operator +=(const Tensor<T2, orders ...>& obj) Stew_noexcept_auto
        {
            for(std::size_t i = 0; i < elements.size(); ++i)
            {
                elements[i] += obj.elements[i];
            }
            return *this;
        }

        template<class T2>
        constexpr Tensor& operator -=(const Tensor<T2, orders ...>& obj) Stew_noexcept_auto
        {
            for(std::size_t i = 0; i < elements.size(); ++i)
            {
                elements[i] -= obj.elements[i];
            }
            return *this;
        }

        template<class T2>
        constexpr Tensor& operator *=(const Tensor<T2, orders ...>& obj) Stew_noexcept_auto
        {
            for(std::size_t i = 0; i < elements.size(); ++i)
            {
                elements[i] *= obj.elements[i];
            }
            return *this;
        }
        
        template<class T2>
        constexpr Tensor& operator /=(const Tensor<T2, orders ...>& obj) Stew_noexcept_auto
        {
            for(std::size_t i = 0; i < elements.size(); ++i)
            {
                elements[i] /= obj.elements[i];
            }
            return *this;
        }

        friend constexpr auto operator-(const Tensor& obj) Stew_noexcept_auto
        {
            Tensor ret{obj};
            for(std::size_t i = 0; i < obj.elements.size(); ++i)
            {
                ret.elements[i] = -obj.elements[i];
            }
            return ret;
        }

        friend constexpr auto norm2(const Tensor& obj) Stew_noexcept_auto
        {
            auto&& ret = obj.elements[0];
            for(std::size_t i = 1; i < obj.elements.size(); ++i)
            {
                ret = std::move(ret) + obj.elements[i] * obj.elements[i];
            }
            return ret;
        }

        friend constexpr auto norm(const Tensor& obj) Stew_noexcept_auto
        {
            return sqrt(norm2(obj));
        }

        template<class T2>
        friend constexpr auto operator +(const Tensor& l, const Tensor<T2, orders ...>& r) Stew_noexcept_auto
        {
            Tensor<decltype(l.elements[0] + r.elements[0])> ret{l};
            for(std::size_t i = 0; i < ret.elements.size(); ++i)
            {
                ret.elements[i] = std::move(ret.elements[i]) + r;
            }
            return ret;
        }

        template<class T2>
        friend constexpr auto operator -(const Tensor& l, const Tensor<T2, orders ...>& r) Stew_noexcept_auto
        {
            Tensor<decltype(l.elements[0] + r.elements[0])> ret{l};
            for(std::size_t i = 0; i < ret.elements.size(); ++i)
            {
                ret.elements[i] = std::move(ret.elements[i]) - r;
            }
            return ret;
        }

        template<class T2>
        requires std::is_scalar_v<T2>
        friend constexpr auto operator *(const T2& l, const Tensor& r) Stew_noexcept_auto
        {
            Tensor ret{r};
            for(std::size_t i = 0; i < ret.elements.size(); ++i)
            {
                ret.elements[i] = l * std::move(ret.elements[i]);
            }
            return ret;
        }

        template<class T2>
        requires std::is_scalar_v<T2>
        friend constexpr auto operator *(const Tensor& l, const T2& r) Stew_noexcept_auto
        {
            Tensor ret{l};
            for(std::size_t i = 0; i < ret.elements.size(); ++i)
            {
                ret.elements[i] = std::move(ret.elements[i]) * r;
            }
            return ret;
        }

        template<class T2>
        requires std::is_scalar_v<T2>
        friend constexpr auto operator /(const Tensor& l, const T2& r) Stew_noexcept_auto
        {
            Tensor ret{l};
            for(std::size_t i = 0; i < ret.elements.size(); ++i)
            {
                ret.elements[i] = std::move(ret.elements[i]) / r;
            }
            return ret;
        }

        template<class Traits>
        friend constexpr auto& operator <<(std::basic_ostream<char, Traits>& os, const Tensor& obj)
        {
            os << "{";
            for(std::size_t i = 0; i < obj.elements.size(); ++i)
            {
               os << obj.elements[i] << ", ";
            }
            return os << "}";
        }
    };
}