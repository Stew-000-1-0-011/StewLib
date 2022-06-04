#pragma once
#include <tuple>
#include <utility>
#include <concepts>
#include <type_traits>

namespace StewLib
{
    namespace Implement::NamedParam
    {
        struct TagBase{};
        struct HolderBase{};
        template<class Tag_>
        struct NamedParamHolder;
    }

    template<class T>
    concept IsNamedParamTagC = std::derived_from<T, Implement::NamedParam::TagBase>;

    template<class T>
    concept IsNamedParamHolderC = std::derived_from<T, Implement::NamedParam::HolderBase>;

    template<class Name_, class Type_>
    struct NamedParamTag final : Implement::NamedParam::TagBase
    {
        using Name = Name_;
        using Type = Type_;

        template<class Arg>
        constexpr auto operator =(Arg&& arg) noexcept
        {
            return Implement::NamedParam::NamedParamHolder<NamedParamTag>(std::forward<Arg>(arg));
        }
    };

    namespace Implement::NamedParam
    {
        template<class Tag_>
        requires IsNamedParamTagC<Tag_>
        struct NamedParamHolder<Tag_> final : Implement::NamedParam::HolderBase
        {
            using Tag = Tag_;

            Tag::Type value;

            explicit constexpr NamedParamHolder(Tag::Type value) noexcept:
                value{std::forward<typename Tag::Type>(value)}
            {}

            constexpr NamedParamHolder(NamedParamHolder&& obj) noexcept:
                value{std::forward<typename Tag::Type>(obj.value)}
            {}
        };
    }

    template<class ... Tags>
    requires (IsNamedParamTagC<Tags> && ...)
    struct NamedParamList final
    {
        std::tuple<Implement::NamedParam::NamedParamHolder<Tags> ...> holders;

        template<class ... Holders>
        requires (std::same_as<Holders, std::remove_reference_t<Holders>> && ...)
        constexpr NamedParamList(Holders&& ... unordered_holders) noexcept:
            holders{std::get<Implement::NamedParam::NamedParamHolder<Tags>>(std::tuple<std::remove_reference_t<Holders> ...>(std::move(unordered_holders) ...)) ...}
        {}

        template<class Tag>
        requires IsNamedParamTagC<Tag>
        constexpr decltype(auto) operator [](const Tag) noexcept
        {
            // これstd::get<Implement::NamedParam::NamedParamHolder<Tag>>(holders).valueをそのまま返してもエラー吐かないのなんでだ...？
            return static_cast<Tag::Type>(std::get<Implement::NamedParam::NamedParamHolder<Tag>>(holders).value);
        }
    };
}


#define stew_define_named_parameter(name, type) \
using name##_tag = StewLib::NamedParamTag<struct impl_##name##_tag_name, type>; \
name##_tag name
