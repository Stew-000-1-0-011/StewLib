// #pragma once 故意

#define Stew_define_has_member(member, func_name)\
\
namespace StewLib\
{\
    namespace Implement::HasMemberImp\
    {\
        template<class T>\
        static constexpr auto has_##func_name(int) noexcept -> decltype(&T::member, bool())\
        {\
            return true;\
        }\
\
        template<class T>\
        static constexpr bool has_##func_name(double) noexcept\
        {\
            return false;\
        }\
    }\
\
    namespace HasMember\
    {\
        template<class T>\
        inline constexpr bool has_##func_name##_v = Implement::HasMemberImp::has_##func_name<T>(0);\
    }\
}\

