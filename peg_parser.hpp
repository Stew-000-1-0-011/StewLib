#pragma once

/*

結局間に合わずCで書き直す暇がなかった。そも、デバッグすらまともにできてないので...まった、再帰できなくないかこれ？
std::functionとか関数ポインタ使うのは絶対避けたいし...なんかいい方法ないかな。

*/

#include <cstddef>
#include <cstdio>
#include <concepts>
#include <variant>
#include <string>
#include <list>
#include <exception>
#include <system_error>
#include <new>
#include <type_traits>
#include <iostream>

namespace StewLib
{
    constexpr struct DontMatch{} dont_match;

    struct ParsedStrings final
    {
        std::variant<std::list<ParsedStrings>, std::string> value;
        enum Index
        {
            nest_parsed_strings,
            string
        };
    };

    // 標準ライブラリが吐く例外のうち、プログラマが悪くないような例外は以下の二つだけだと判断した。
    // それ以外は絶対に吐くな。
    // 私は例外が嫌いだ。
    // 動的確保は失敗を考えるのに、スタックの静的確保
    // (この表現はどうなんだ? 再帰呼び出しとかである処理の使う自動変数領域を動的に変更することはできるし)
    // の失敗を考えなくていいのも不思議だ。
    // 
    // スタックオ－バーフローが検出できないのは、C++という言語の欠陥といっていいのでは？
    // 
    struct ParserError final
    {
        std::variant<std::system_error, std::bad_alloc> value;

        ParserError(const ParserError&) = default;
        ParserError(ParserError&&) = default;
        ParserError& operator =(const ParserError&) = default;
        ParserError& operator =(ParserError&&) = default;
        ~ParserError() = default;

        constexpr ParserError(const std::bad_alloc& e) noexcept:
            value{e}
        {}

        ParserError(const std::system_error& e) noexcept:
            value{std::system_error{e.code(), e.what()}}
        {}

        enum Index
        {
            system_error,
            bad_alloc
        };
    };

    struct ParserReturn final
    {
        std::variant<DontMatch, ParsedStrings, ParserError> value;

        enum Index
        {
            dont_match,
            parsed_strings,
            error
        };
    };

    template <class T>
    concept Parser = requires(T lambda, std::FILE *const fp){{lambda(fp)} noexcept -> std::same_as<ParserReturn>;};

    struct Rewinder final
    {
        std::FILE *const fp;
        std::fpos_t pos{};
        bool active{true};

        Rewinder(std::FILE *const fp):
            fp{fp}
        {
            if(std::fgetpos(fp, &pos))
            {
                throw std::system_error{errno, std::system_category()};
            }
        }

        ~Rewinder() noexcept(false)
        {
            if(active && std::fsetpos(fp, &pos))
            {
                throw std::system_error{errno, std::system_category()};
            }
        }
    };

    namespace PegParserOperators
    {
        template<Parser L, Parser R>
        constexpr Parser auto operator -(L, R) noexcept
        {
            return [](std::FILE *const fp) noexcept -> ParserReturn
            {
                try
                {
                    auto&& l_ret = L{}(fp);

                    switch(l_ret.value.index())
                    {
                    case ParserReturn::dont_match:
                    case ParserReturn::error:
                        return l_ret;

                    case ParserReturn::parsed_strings:
                        ;
                    }

                    ParsedStrings parsed_strings_l{std::list{std::move(std::get<ParsedStrings>(l_ret.value))}};

                    auto&& r_ret = R{}(fp);

                    switch(r_ret.value.index())
                    {
                    case ParserReturn::dont_match:
                    case ParserReturn::error:
                        return r_ret;

                    case ParserReturn::parsed_strings:
                        ;
                    }

                    std::get<ParsedStrings::nest_parsed_strings>(parsed_strings_l.value).push_back(std::move(std::get<ParsedStrings>(r_ret.value)));

                    return {std::move(parsed_strings_l)};
                }
                catch(const std::bad_alloc& e)
                {
                    return {e};
                }
            };
        }

        template<Parser L, Parser R>
        constexpr Parser auto operator /(L, R) noexcept
        {
            return [](std::FILE *const fp) noexcept -> ParserReturn
            {
                try
                {
                    auto&& l_ret = L{}(fp);

                    switch(l_ret.value.index())
                    {
                    case ParserReturn::parsed_strings:
                    case ParserReturn::error:
                        return l_ret;
                    case ParserReturn::dont_match:
                        return R{}(fp);
                    }
                }
                catch(const std::bad_alloc& e)
                {
                    return {e};
                }
            };
        }

        template<Parser X>
        constexpr Parser auto operator *(X) noexcept
        {

            return [](std::FILE *const fp) noexcept -> ParserReturn
            {
                try
                {
                    ParsedStrings ret{};

                    while(true)
                    {
                        auto&& x_ret = X{}(fp);

                        switch(x_ret.value.index())
                        {
                        case ParserReturn::dont_match:
                            return {std::move(ret)};
                        
                        case ParserReturn::parsed_strings:
                            std::get<ParsedStrings::nest_parsed_strings>(ret.value).push_back(std::get<ParserReturn::parsed_strings>(x_ret.value));
                            break;

                        case ParserReturn::error:
                            return x_ret;
                        }
                    }
                }
                catch(const std::bad_alloc& e)
                {
                    return {e};
                }
            };
        }

        template<Parser X>
        constexpr Parser auto operator +(X) noexcept
        {
            return X{} - *(X{});
        }

        template<Parser X>
        constexpr Parser auto operator ~(X) noexcept
        {
            return [](std::FILE *const fp) -> ParserReturn
            {
                try
                {
                    auto&& x_ret = X{}(fp);

                    switch(x_ret.value.index())
                    {
                    case ParserReturn::dont_match:
                        return {ParsedStrings{""}};

                    case ParserReturn::parsed_strings:
                    case ParserReturn::error:
                        return x_ret;
                    }
                }
                catch(const std::bad_alloc& e)
                {
                    return {e};
                }
            };
        }

        template<Parser X>
        constexpr Parser auto operator &(X) noexcept
        {

            return [](std::FILE *const fp) -> ParserReturn
            {
                try
                {
                    Rewinder rewinder{fp};
                    try
                    {
                        auto&& x_ret = X{}(fp);

                        switch(x_ret.value.index())
                        {
                        case ParserReturn::dont_match:
                        case ParserReturn::error:
                            return x_ret;
                        
                        case ParserReturn::parsed_strings:
                            return x_ret;
                        }
                    }
                    catch(const std::bad_alloc& e)
                    {
                        return {e};
                    }
                }
                catch(const std::system_error& e)
                {
                    return {e};
                }
            };
        }

        template<Parser X>
        constexpr Parser auto operator !(X) noexcept
        {
            return [](std::FILE *const fp) noexcept -> ParserReturn
            {
                try
                {
                    Rewinder rewinder{fp};
                    try
                    {
                        auto&& x_ret = X{}(fp);

                        switch(x_ret.value.index())
                        {
                        case ParserReturn::dont_match:
                            return {ParsedStrings{""}};
                        
                        case ParserReturn::parsed_strings:
                            return {dont_match};

                        case ParserReturn::error:
                            return x_ret;
                        }
                    }
                    catch(const std::bad_alloc& e)
                    {
                        return {e};
                    }
                }
                catch(const std::system_error& e)
                {
                    return {e};
                }
            };
        }
    }

    namespace Implement::PegParserImp
    {
        struct PegStringLiteralBase{};

        template<class T>
        concept IsPegStringLiteral = std::is_base_of_v<PegStringLiteralBase, T>;

        inline int fgetc(std::FILE *const fp) noexcept(false)
        {
            if(int c = std::fgetc(fp); c == EOF && !std::feof(fp))
            {
                throw std::system_error{errno, std::system_category()};
            }
            else return c;
        }
    }

    template<std::size_t n>
    struct PegStringLiteral final : Implement::PegParserImp::PegStringLiteralBase
    {
        char str[n];

        operator const decltype(str)& () const noexcept
        {
            return str;
        }

        operator decltype(str)& () noexcept
        {
            return str;
        }
    };

    template<const Implement::PegParserImp::IsPegStringLiteral auto& str>
    constexpr Parser auto sentence() noexcept
    {
        return [](std::FILE *const fp) noexcept ->ParserReturn
        {
            try
            {
                Rewinder rewinder{fp};

                for(std::size_t i = 0; i < sizeof(str) - 1; ++i)
                {
                    try
                    {
                        if(int c = Implement::PegParserImp::fgetc(fp); c == EOF || c != str[i])
                        {
                            return {dont_match};
                        }
                    }
                    catch(const std::system_error& e)
                    {
                        return {e};
                    }
                }

                try
                {
                    rewinder.active = false;
                    return {ParsedStrings{str}};
                }
                catch(const std::bad_alloc& e)
                {
                    return {e};
                }
            }
            catch(const std::system_error& e)
            {
                return {e};
            }
        };
    }

    template<char ... charset>
    constexpr auto star_character = [](std::FILE *const fp) noexcept -> ParserReturn
    {
        try
        {
            Rewinder rewinder{fp};
            try
            {
                std::string ret{};
                while(true)
                {
                    try
                    {
                        if(int c = Implement::PegParserImp::fgetc(fp); ((c != charset) && ...))
                        {
                            return {ParsedStrings{std::move(ret)}};
                        }
                        else
                        {
                            ret += c;
                        }
                    }
                    catch(const std::system_error& e)
                    {
                        return {e};
                    }
                }
            }
            catch(const std::bad_alloc& e)
            {
                return {e};
            }
        }
        catch(const std::system_error& e)
        {
            return {e};
        }
    };

    inline void print_parsed_strings(const ParsedStrings& strs, const std::size_t indent)
    {
        switch(strs.value.index())
        {
        case ParsedStrings::nest_parsed_strings:
            for(const auto& element_strs : std::get<ParsedStrings::nest_parsed_strings>(strs.value))
            {
                print_parsed_strings(element_strs, indent + 1);
            }
            break;
        
        case ParsedStrings::string:
            for(std::size_t i = 0; i < indent; ++i)
            {
                std::cout << "  ";
            }
            std::cout << "{\n";
            for(std::size_t i = 0; i < indent; ++i)
            {
                std::cout << "  ";
            }
            std::cout << "  " << std::get<ParsedStrings::string>(strs.value) << "\n";
            for(std::size_t i = 0; i < indent; ++i)
            {
                std::cout << "  ";
            }
            std::cout << "}" << std::endl;
        }
    }

    inline void print_parser_return(const ParserReturn& ret)
    {
        switch(ret.value.index())
        {
        case ParserReturn::dont_match:
            std::cout << "don\'t match." << std::endl;
            break;

        case ParserReturn::parsed_strings:
            print_parsed_strings(std::get<ParserReturn::parsed_strings>(ret.value), 0);
            break;

        case ParserReturn::error:
            std::cout << "parser error." << std::endl;
        }
    }

    constexpr auto star_whitespace = star_character<' ', '\t'>;
    constexpr auto star_newline = star_character<'\r', '\n'>;
    constexpr auto star_whitespace_newline = star_character<' ', '\t', '\r', '\n'>;
    constexpr auto star_digit10 = star_character<'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'>;
    constexpr auto star_digit2 = star_character<'0', '1'>;
    constexpr auto star_digit16 = star_character<'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'>;
}