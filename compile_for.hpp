#pragma once

#include <utility>
#include <type_traits>
#ifdef __cpp_lib_concepts
#include <concepts>
#endif


namespace StewLib
{
    enum CompileForBreak : char
    {};

    // template<class T>
    // struct CompileForIter final
    // {
    //     using IterType = T;
    //     inline static std::remove_reference_t<T> dummy{};

    //     T&& iter{std::forward<T>(dummy)};
    //     bool break_flag{false};

    //     constexpr CompileForIter(T&& iter) noexcept:
    //         iter{std::forward<T>(iter)}
    //     {}

    //     constexpr CompileForIter(CompileForBreak) noexcept:
    //         break_flag{true}
    //     {}

    //     constexpr CompileForIter() noexcept:
    //         iter{std::forward<T>(dummy)}
    //     {}
    // };

    template<class T>
#ifdef __cpp_concepts
    requires std::default_initializable<T>
#endif
    struct CompileForIter final
    {
        using IterType = T;

        T iter{};
        bool break_flag{false};

        constexpr CompileForIter(T&& iter) noexcept:
            iter{std::forward<T>(iter)}
        {}

        constexpr CompileForIter(CompileForBreak) noexcept:
            break_flag{true}
        {}

        constexpr CompileForIter() = default;
    };

    namespace Implement::CompileForImp
    {
#ifdef __cpp_concepts
        template<class T>
        struct is_compile_for_t final : std::false_type
        {};

        template<class T>
        struct is_compile_for_t<CompileForIter<T>> final : std::true_type
        {};

        template<class T>
        concept CompileForC = is_compile_for_t<T>::value;
#endif

        template<class BodyF, class Iter>
#ifdef __cpp_concepts
        requires requires(BodyF body_f, Iter iter)
        {
            {body_f(iter)} noexcept -> CompileForC;
        }
#endif
        constexpr void run(BodyF&& body_f, Iter&& iter) noexcept
        {
            auto&& iter_opt = body_f(iter);
            if(iter_opt.break_flag) return;
            run(body_f, std::forward<typename std::remove_cvref_t<decltype(iter_opt)>::IterType>(iter_opt.iter));
        }
    }

    template<class BodyF, class Iter>
    constexpr void compile_for(BodyF&& body_f, Iter&& iter) noexcept
    {
        Implement::CompileForImp::run(body_f, std::forward<Iter>(iter));
    }
}