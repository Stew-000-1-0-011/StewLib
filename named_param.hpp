// 参照型でないものも持てないことはない。が、何回かコンストラクタが呼び出されるので注意。

#pragma once
#include <tuple>
#include <utility>
#include <concepts>
#include <type_traits>

#include "type_holder.hpp"

namespace StewLib
{
    namespace Implement::NamedParamImp
    {
        struct ForwardRef;
        struct NonAutoTagBase{};
        struct ForwardRefTagBase{};
        struct WithDefaultValueBase{};
        struct NonDummyHolderBase{};
        struct DummyHolderBase{};

        template<class T>
        concept IsNonAutoTag = std::derived_from<std::remove_cvref_t<T>, Implement::NamedParamImp::NonAutoTagBase>;

        template<class T>
        concept IsForwardRefTag = std::derived_from<std::remove_cvref_t<T>, Implement::NamedParamImp::ForwardRefTagBase>;

        template<class T>
        concept IsTag = IsNonAutoTag<T> || IsForwardRefTag<T>;

        template<class T>
        concept IsWithDefaultValue = std::derived_from<std::remove_cvref_t<T>, Implement::NamedParamImp::WithDefaultValueBase>;

        template<class T>
        concept IsNonDummyHolder = std::derived_from<std::remove_cvref_t<T>, Implement::NamedParamImp::NonDummyHolderBase>;

        template<class T>
        concept IsDummyHolder = std::derived_from<std::remove_cvref_t<T>, Implement::NamedParamImp::DummyHolderBase>;

        template<class T>
        concept IsHolder = IsNonDummyHolder<T> || IsDummyHolder<T>;

        template<IsTag Tag_>
        struct NonDummyHolder;

        template<class Identifier, class Type_, IsPred auto pred = empty_pred>
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
                return NonDummyHolder<Tag<Identifier, Arg&&>>(std::forward<Arg>(arg));
            }
        };

        template<class Identifier, class Type_, auto default_factory_, IsPred auto pred = empty_pred>
        struct TagWithDefault;

        template<class Identifier_, class Type_, auto default_factory_>
        requires (!std::same_as<Type_, ForwardRef>)
        struct TagWithDefault<Identifier_, Type_, default_factory_> final : NonAutoTagBase, WithDefaultValueBase
        {
            using Identifier = Identifier_;
            using Type = Type_;

            static decltype(auto) default_factory()
            {
                return default_factory_();
            }

            constexpr decltype(auto) operator =(Type arg) const
            {
                return NonDummyHolder<Tag<typename TagWithDefault::Identifier, TagWithDefault::Type>>(std::forward<Type>(arg));
            }
        };

        template<class Identifier_, auto default_factory_, IsPred auto pred>
        struct TagWithDefault<Identifier_, ForwardRef, default_factory_, pred> final : ForwardRefTagBase, WithDefaultValueBase
        {
            using Identifier = Identifier_;
            using Type = ForwardRef;

            static decltype(auto) default_factory()
            {
                return default_factory_();
            }

            template<class Arg>
            requires (pred.template operator()<Arg&&>())
            constexpr decltype(auto) operator =(Arg&& arg) const
            {
                return NonDummyHolder<Tag<Identifier, Arg&&>>(std::forward<Arg>(arg));
            }
        };

        template<IsTag Tag_>
        struct NonDummyHolder final : NonDummyHolderBase
        {
            using Type = typename Tag_::Type;
            Type value;

        public:
            using Tag = Tag_;

            template<class Args>
            explicit constexpr NonDummyHolder(Args&& value):
                value{std::forward<Args>(value)}
            {}

            NonDummyHolder(const NonDummyHolder&) = delete;

            constexpr NonDummyHolder(NonDummyHolder&& obj):
                value{std::forward<decltype(obj.value)>(obj.value)}
            {}

            constexpr decltype(auto) get_value() const noexcept
            {
                return static_cast<decltype(value)>(value);
            }
        };

        template<class Tag_>
        struct DummyHolder;

        template<IsTag Tag_>
        requires IsWithDefaultValue<Tag_>
        struct DummyHolder<Tag_> final : DummyHolderBase
        {
            using Tag = Tag_;

            constexpr decltype(auto) get_value() const noexcept
            {
                return Tag::default_factory();
            }
        };

        template<IsTag Tag_>
        requires (!IsWithDefaultValue<Tag_>)
        struct DummyHolder<Tag_> final : DummyHolderBase
        {
            using Tag = Tag_;

            constexpr DummyHolder get_value() const noexcept
            {
                return {};
            }
        };

        constexpr auto compare = []<IsTag Tag, IsHolder Holder>() consteval noexcept
        {
            if constexpr(std::same_as<typename Tag::Identifier, typename std::remove_cvref_t<Holder>::Tag::Identifier>)
            {
                return true;
            }
            else
            {
                return false;
            }
        };

        template<IsTag Tag_>
        constexpr auto compare_with_tag = []<IsHolder Holder>() consteval noexcept{return compare.template operator()<Tag_, Holder>();};
        
        template<IsHolder Holder>
        constexpr auto compare_with_holder = []<IsTag Tag>() consteval noexcept{return compare.template operator()<Tag, Holder>();};

        template<IsTag Tag_>
        constexpr auto compare_with_tag_nondummy = []<IsHolder Holder>() consteval noexcept{return compare.template operator()<Tag_, Holder>() && IsNonDummyHolder<Holder>;};

        template<class Tag_, class ... Holders>
        struct RearrangeHoldersType;

        template<IsTag Tag_, IsHolder ... Holders>
        requires (std::is_rvalue_reference_v<Holders&&> && ...) &&
            (static_cast<bool>(TypeHolder<Holders ...>::size) && TypeHolder<Holders ...>::template is_only_one<compare_with_tag<Tag_>>)
        struct RearrangeHoldersType<Tag_, Holders ...>
        {
            using Type = typename TypeHolder<Holders ...>::template GetTypeT<0, compare_with_tag<Tag_>>;
        };

        template<IsTag Tag_, IsHolder ... Holders>
        requires (std::is_rvalue_reference_v<Holders&&> && ...) &&
            (!TypeHolder<Holders ...>::size || !TypeHolder<Holders ...>::template is_only_one<compare_with_tag<Tag_>>) &&
            IsWithDefaultValue<Tag_> &&
            IsNonAutoTag<Tag_>
        struct RearrangeHoldersType<Tag_, Holders ...>
        {
            using Type = NonDummyHolder<Tag<typename Tag_::Identifier, decltype(Tag_::default_factory())>>;
        };

        template<IsTag Tag_, IsHolder ... Holders>
        requires (std::is_rvalue_reference_v<Holders&&> && ...) &&
            (!TypeHolder<Holders ...>::size || !TypeHolder<Holders ...>::template is_only_one<compare_with_tag<Tag_>>) &&
            IsWithDefaultValue<Tag_> &&
            IsForwardRefTag<Tag_>
        struct RearrangeHoldersType<Tag_, Holders ...>
        {
            using Type = NonDummyHolder<Tag<typename Tag_::Identifier, decltype(Tag_::default_factory())>>;
        };

        template<IsTag Tag_, IsHolder ... Holders>
        requires (std::is_rvalue_reference_v<Holders&&> && ...) &&
            (!TypeHolder<Holders ...>::size || !TypeHolder<Holders ...>::template is_only_one<compare_with_tag<Tag_>>) &&
            (!IsWithDefaultValue<Tag_>)
        struct RearrangeHoldersType<Tag_, Holders ...>
        {
            using Type = DummyHolder<Tag_>;
        };

        template<IsTag Tag_, IsHolder ... Holders>
        requires (std::is_rvalue_reference_v<Holders&&> && ...)
        inline constexpr decltype(auto) rearrange_holders_value(std::tuple<Holders&& ...>& holders_tup)
        {
            using TypeHolderHolders = TypeHolder<Holders ...>;

            if constexpr(TypeHolderHolders::size)
            {
                if constexpr(TypeHolderHolders::template is_only_one<compare_with_tag<Tag_>>)
                {
                    return std::get<TypeHolderHolders::template get_index<compare_with_tag<Tag_>>()>(holders_tup);
                }
                else
                {
                    return DummyHolder<Tag_>();
                }
            }
            else
            {
                return DummyHolder<Tag_>();
            }
        }
    }

    auto tags(const Implement::NamedParamImp::IsTag auto& ... tags)
    {
        return TypeHolder<std::remove_cvref_t<decltype(tags)> ...>();
    }

    template<Implement::NamedParamImp::IsHolder ... Holders>
    class NamedParamList;

    template<Implement::NamedParamImp::IsTag ... Tags, Implement::NamedParamImp::IsHolder ... Holders>
    requires
        (std::is_rvalue_reference_v<Holders&&> && ...) &&
        TypeHolder<Holders ...>::is_non_duplicated &&
        TypeHolder<Tags ...>::is_non_duplicated &&
        (TypeHolder<Tags ...>::template is_only_one<Implement::NamedParamImp::compare_with_holder<Holders>> && ...)
    inline constexpr auto make_named_params(TypeHolder<Tags ...>, Holders&& ... holders)
    {
        std::tuple<Holders&& ...> holders_tup{std::move(holders) ...};

        // std::cout << "make_named_params" << std::endl;
        // ((std::cout << "make_named_params" << holders.get_value() << std::endl), ...);

        return NamedParamList<typename Implement::NamedParamImp::RearrangeHoldersType<Tags, Holders ...>::Type ...>
        {
            std::move(Implement::NamedParamImp::rearrange_holders_value<Tags>(holders_tup)) ...
        };
    }

    template<Implement::NamedParamImp::IsHolder ... Holders>
    class NamedParamList final
    {
        template<Implement::NamedParamImp::IsTag ... Tags2, Implement::NamedParamImp::IsHolder ... Holders2>
        requires
            (std::is_rvalue_reference_v<Holders2&&> && ...) &&
            TypeHolder<Holders2 ...>::is_non_duplicated &&
            TypeHolder<Tags2 ...>::is_non_duplicated &&
            (TypeHolder<Tags2 ...>::template is_only_one<Implement::NamedParamImp::compare_with_holder<Holders2>> && ...)
        friend inline constexpr auto make_named_params(TypeHolder<Tags2 ...>, Holders2&& ... holders);

        using TypeHolderHolders = TypeHolder<Holders ...>;

        std::tuple<Holders ...> holders;

        template<Implement::NamedParamImp::IsHolder ... ArgHolders>
        constexpr NamedParamList(ArgHolders&& ... arg_holders):
            holders{std::forward<decltype(arg_holders.get_value())>(arg_holders.get_value()) ...}
        {}

    public:
        template<Implement::NamedParamImp::IsTag Tag>
        requires Implement::NamedParamImp::IsNonDummyHolder<typename TypeHolderHolders::template GetTypeT<0, Implement::NamedParamImp::compare_with_tag<Tag>>>
        constexpr decltype(auto) operator [](const Tag&)
        {
            return std::get<TypeHolderHolders::template get_index<Implement::NamedParamImp::compare_with_tag<Tag>>()>(holders).get_value();
        }

        template<Implement::NamedParamImp::IsTag Tag>
        static consteval bool is_in(const Tag&) noexcept
        {
            return TypeHolderHolders::template is_only_one<Implement::NamedParamImp::compare_with_tag_nondummy<Tag>>;
        }
    };

    using Implement::NamedParamImp::IsHolder;
}

#if 0
#define stew_define_named_parameter(name, type)\
using name##_tag = StewLib::Implement::NamedParamImp::Tag<struct impl_##name##_tag_name, type>;\
inline static const name##_tag name{}

#define stew_define_named_auto_parameter(name, ...)\
using name##_tag = StewLib::Implement::NamedParamImp::Tag<struct impl_##name##_tag_name, StewLib::Implement::NamedParamImp::ForwardRef __VA_OPT__(, __VA_ARGS__)>;\
inline static const name##_tag name{}

#define stew_define_named_parameter_with_default(name, type, default_factory_)\
using name##_tag = StewLib::Implement::NamedParamImp::TagWithDefault<struct impl_##name##_tag_name, type, default_factory_>;\
inline static const name##_tag name{}

#define stew_define_named_auto_parameter_with_default(name, default_factory_, ...)\
using name##_tag = StewLib::Implement::NamedParamImp::TagWithDefault<struct impl_##name##_tag_name, StewLib::Implement::NamedParamImp::ForwardRef, default_factory_ __VA_OPT__(, __VA_ARGS__)>;\
inline static const name##_tag name{}
#endif

#define stew_define_named_parameter(name, type)\
inline static constexpr StewLib::Implement::NamedParamImp::Tag<struct impl_##name##_tag_name, type> name{}

#define stew_define_named_auto_parameter(name, ...)\
inline static constexpr StewLib::Implement::NamedParamImp::Tag<struct impl_##name##_tag_name, StewLib::Implement::NamedParamImp::ForwardRef __VA_OPT__(, __VA_ARGS__)> name{}

#define stew_define_named_parameter_with_default_inner(name, type, default_factory_)\
inline static constexpr StewLib::Implement::NamedParamImp::TagWithDefault<struct impl_##name##_tag_name, type, default_factory_> name{}

#define stew_define_named_auto_parameter_with_default_inner(name, default_factory_, ...)\
inline static constexpr StewLib::Implement::NamedParamImp::TagWithDefault<struct impl_##name##_tag_name, StewLib::Implement::NamedParamImp::ForwardRef, default_factory_ __VA_OPT__(, __VA_ARGS__)> name{}

#define stew_define_named_parameter_with_default(name, type, lambda)\
inline static constexpr auto name##_default = lambda;\
stew_define_named_parameter_with_default_inner(name, type, name##_default)

#define stew_define_named_auto_parameter_with_default(name, lambda, ...)\
inline static constexpr auto name##_default = lambda;\
stew_define_named_auto_parameter_with_default_inner(name, name##_default __VA_OPT__(, __VA_ARGS__))
