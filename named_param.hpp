// 参照型でないものも持てないことはない。が、moveされるので注意。

#pragma once
#include <tuple>
#include <utility>
#include <concepts>
#include <type_traits>

#include "type_holder.hpp"

namespace StewLib
{
    struct ForwardRef;

    namespace Implement::NamedParamImp
    {
        struct NonAutoTagBase{};
        struct ForwardRefTagBase{};
        struct NonDummyHolderBase{};
        struct DummyHolderBase{};

        template<class Tag>
        struct NonDummyHolder;

        template<class T>
        concept IsNonAutoTag = std::derived_from<std::remove_cvref_t<T>, Implement::NamedParamImp::NonAutoTagBase>;

        template<class T>
        concept IsForwardRefTag = std::derived_from<std::remove_cvref_t<T>, Implement::NamedParamImp::ForwardRefTagBase>;

        template<class T>
        concept IsTag = IsNonAutoTag<T> || IsForwardRefTag<T>;

        template<class T>
        concept IsNonDummyHolder = std::derived_from<std::remove_cvref_t<T>, Implement::NamedParamImp::NonDummyHolderBase>;

        template<class T>
        concept IsDummyHolder = std::derived_from<std::remove_cvref_t<T>, Implement::NamedParamImp::DummyHolderBase>;

        template<class T>
        concept IsHolder = IsNonDummyHolder<T> || IsDummyHolder<T>;

        template<class Identifier, class Type_, IsPred auto pred = empty_lambda>
        struct Tag;

        template<class Identifier_, class Type_>
        requires (!std::same_as<Type_, ForwardRef>)
        struct Tag<Identifier_, Type_> final : NonAutoTagBase
        {
            using Identifier = Identifier_;
            using Type = Type_;

            constexpr decltype(auto) operator =(Type arg) const
            {
                return NonDummyHolder<Tag>(std::forward<Type>(arg));
            }
        };

        template<class Identifier_, IsPred auto pred>
        struct Tag<Identifier_, ForwardRef, pred> final : ForwardRefTagBase
        {
            using Identifier = Identifier_;
            using Type = ForwardRef;

            template<class Arg>
            requires (pred.template operator()<Arg&&>())
            constexpr decltype(auto) operator =(Arg&& arg) const
            {
                return Holder<Tag<Identifier, Arg&&>>(std::forward<Arg>(arg));
            }
        };

        template<IsNonAutoTag NonAutoTag>
        struct NonDummyHolder<NonAutoTag> final : NonDummyHolderBase
        {
        private:
            using Type = NonAutoTag::Type;

        public:
            using Tag = NonAutoTag;
            Type value;

            explicit constexpr NonDummyHolder(Type value):
                value{std::forward<Type>(value)}
            {}

            constexpr NonDummyHolder(NonDummyHolder&& obj) noexcept:
                value{std::forward<Type>(obj.value)}
            {}
        };

        template<IsTag Tag_>
        struct DummyHolder final : DummyHolderBase
        {
            using Tag = Tag_;
        };

        constexpr auto compare = []<IsTag Tag, IsHolder Holder>() consteval noexcept
        {
            if constexpr(IsNonDummyHolder<Holder> && std::same_as<typename Tag::Identifier, typename std::remove_cvref_t<Holder>::Tag::Identifier>)
            {
                return true;
            }
            else
            {
                return false;
            }
        };

        template<IsTag Tag>
        constexpr auto compare_with_tag = []<IsHolder Holder>() consteval noexcept{return compare.template operator()<Tag, Holder>();};
        
        template<IsHolder Holder>
        constexpr auto compare_with_holder = []<IsTag Tag>() consteval noexcept{return compare.template operator()<Tag, Holder>();};

        template<IsTag Tag, IsHolder ... Holders>
        inline constexpr decltype(auto) make_named_param_helper(std::tuple<Holders ...>& holder_tup)
        {
            using TypeHolderHolders = TypeHolder<std::remove_cvref_t<Holders> ...>;

            if constexpr(TypeHolderHolders::template is_only_one<compare_with_tag<Tag>>)
            {
                return std::forward<typename TypeHolderHolders::template GetTypeT<0, compare_with_tag<Tag>>>
                    (std::get<TypeHolderHolders::template get_index<compare_with_tag<Tag>>()>(holder_tup));
            }
            else
            {
                return DummyHolder<Tag>();
            }
        }
    }

    template<class TypeHolderTags, class ... Holders>
    class NamedParamList;

    template<class TypeHolderTags, class ... Holders>
    requires 
        (sizeof...(Holders) != 0) &&
        TypeHolder<std::remove_cvref_t<Holders> ...>::is_non_duplicated &&
        TypeHolderTags::is_non_duplicated &&
        (TypeHolderTags::template is_only_one<Implement::NamedParamImp::compare_with_holder<Holders>> && ...)
    inline constexpr auto make_named_params(Holders&& ... holders)
    {
        return [&holders ...]<std::size_t ... indexs>(std::index_sequence<indexs ...>) constexpr
        {
            std::tuple<Holders&& ...> holders_tup{std::forward<Holders>(holders) ...};
            return NamedParamList
            {
                TypeHolderTags(),
                Implement::NamedParamImp::make_named_param_helper<typename TypeHolderTags::GetTypeT<indexs>>(holders_tup) ...
            };
        }(std::make_index_sequence<TypeHolderTags::size>());
    }

    template<class TypeHolderTags>
    requires TypeHolderTags::is_non_duplicated
    inline constexpr auto make_named_params() noexcept
    {
        return NamedParamList<TypeHolderTags>{};
    }

    template<class TypeHolderTags, class ... Holders>
    class NamedParamList final
    {
        template<class TypeHolderTags2, class ... Holders2>
        requires
            (sizeof...(Holders2) != 0) &&
            TypeHolder<std::remove_cvref_t<Holders2> ...>::is_non_duplicated &&
            TypeHolderTags2::is_non_duplicated &&
            (TypeHolderTags2::template is_only_one<Implement::NamedParamImp::compare_with_holder<Holders2>> && ...)
        friend constexpr auto make_named_params(Holders2&& ... holders);

        using TypeHolderHolders = TypeHolder<std::remove_cvref_t<Holders> ...>;

        std::tuple<Holders&& ...> holders;

        constexpr NamedParamList(TypeHolderTags, Holders&& ... holders):
            holders{std::forward<Holders>(holders) ...}
        {}

    public:
        template<Implement::NamedParamImp::IsTag Tag>
        requires Implement::NamedParamImp::IsNonDummyHolder<typename TypeHolderHolders::template GetTypeT<0, Implement::NamedParamImp::compare_with_tag<Tag>>>
        constexpr decltype(auto) operator [](const Tag)
        {
            return std::get<TypeHolderHolders::template get_index<Implement::NamedParamImp::compare_with_tag<Tag>>()>(holders).value;
        }

        template<Implement::NamedParamImp::IsTag Tag>
        static consteval bool is_in(const Tag) noexcept
        {
            return TypeHolderHolders::template is_only_one<Implement::NamedParamImp::compare_with_tag<Tag>>;
        }
    };

    template<class TypeHolderArgs>
    class NamedParamList<TypeHolderArgs> final
    {
        template<class TypeHolderTags2>
        requires TypeHolderTags2::is_non_duplicated
        friend constexpr auto make_named_params() noexcept;

        NamedParamList() = default;

    public:
        template<Implement::NamedParamImp::IsTag Tag>
        constexpr decltype(auto) operator [](const Tag) = delete;

        template<Implement::NamedParamImp::IsTag Tag>
        static consteval bool is_in(const Tag) noexcept
        {
            return false;
        }
    };
}

#define stew_define_named_parameter(name, type, ...)\
using name##_tag = StewLib::Implement::NamedParamImp::Tag<struct impl_##name##_tag_name, type __VA_OPT__(, __VA_ARGS__)>;\
constexpr name##_tag name
