#pragma once


#include <cstddef>
#include <cmath>
#include <utility>
#include <ostream>
#include <span>

#include "../array.hpp"
#include "../noexcept_auto.hpp"
#include "functions.hpp"

namespace StewLib::Math
{
    namespace Implement::TensorImp
    {
        template<std::size_t ... orders>
        constexpr std::size_t calc_index(const std::integral auto ... indexs) noexcept
        requires (0 < sizeof...(indexs) && sizeof...(indexs) <= sizeof...(orders))
        {
            constexpr Array<std::size_t, sizeof...(orders)> dim_sizes
            {
                []
                {
                    Array<std::size_t, sizeof...(orders)> ret = {orders ...};
                    std::size_t tmp1 = 1;
                    for(std::size_t i = ret.size(); i > 0 && (--i, 1);)
                    {
                        std::size_t tmp2 = tmp1;
                        tmp1 *= ret[i];
                        ret[i] = tmp2;
                    }
                    return ret;
                }()
            };

            Array<std::size_t, sizeof...(indexs)> indexs_arr{static_cast<std::size_t>(indexs) ...};
            std::size_t ret = 0;
            for(std::size_t i = 0; i < indexs_arr.size(); ++i)
            {
                ret += indexs_arr[i] * dim_sizes[i];
            }
            return ret;
        }

        template<bool is_rvalue, class Elem, std::size_t ... orders>
        struct RefTensor
        {
            static constexpr Array<std::size_t, sizeof...(orders)> orders_array{orders ...};

            std::span<Elem, (orders * ...)> elements{};

            constexpr RefTensor(const std::span<Elem, (orders * ...)> elements) noexcept:
                elements{elements}
            {}

            constexpr RefTensor()

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
                    return Implement::TensorImp::RefTensor<false, const Elem, orders_array[indexs_rest] ...>{std::span<const Elem, (orders_array[indexs_rest] * ...)>{elements.cbegin() + Implement::TensorImp::calc_index<orders ...>(indexs ...), (orders_array[indexs_rest] * ...)}};
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
                for(std::size_t i = 0; i < elements.size(); ++i)
                {
                    elements[i] = obj.elements[i];
                }
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
        };
    }
}