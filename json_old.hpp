#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <optional>
#include <variant>
#include <vector>
#include <list>
#include <fstream>
#include <stdexcept>
#include <limits>
#include <concepts>

namespace StewLib
{
    struct JsonObject;

    namespace Implement::JsonImp
    {
        template<template<class> class Container>
        struct JsonStructBody final
        {
            // 不完全型をサポート until C++17
            Container<JsonObject> json_elements{};
        };

        using JsonStruct = std::variant<JsonStructBody<std::vector>, JsonStructBody<std::list>>;

        using JsonValue = std::variant<std::nullopt_t, bool, std::vector<double>, double, std::string, JsonStruct>;
    }

    struct JsonObject final
    {
        JsonObject * parent;
        std::string identifier;
        Implement::JsonImp::JsonValue value;

        constexpr std::string get_path() const
        {
            if(parent) return parent->get_path() + '/' + identifier;
            else return "";
        }
    };

    namespace Implement::JsonImp
    {
        constexpr bool is_whitespace(const char c) noexcept
        {
            return c == ' ' || c == '\t' || c == '\n' || c == '\r';
        }

        constexpr std::runtime_error json_parse_error(std::ifstream& ifs)
        {
            const auto&& pos = ifs.tellg();
            ifs.seekg(0);
            unsigned int line = 1;
            while(ifs.tellg() != pos)
            {
                char c;
                ifs >> c;
                if(c == '\n')
                {
                    if(line < std::numeric_limits<unsigned int>::max) ++line;
                }
            }
            std::string ret{"StewLib: Error: json parse error is occurred when parsing LINE "};
            ret += std::to_string(line) + ".\n";
            return ret;
        }

        inline std::variant<std::string, std::runtime_error> parse_string(std::ifstream& ifs)
        {
            std::string ret{};

            while(true)
            {
                char c{-1};
                ifs >> c;

                if(c == EOF || c < 0 || c > 127)
                {
                    return json_parse_error(ifs);
                }
                else if(c == '\"')
                {
                    break;
                }
                else
                {
                    ret += c;
                }
            }

            return ret;
        }

        inline std::variant<>

        inline std::variant<char, std::runtime_error> parse_whitespace(std::ifstream& ifs, const std::same_as<char> auto ... end_characters)
        {
            while(true)
            {
                char c{-1};
                ifs >> c;
                if(is_whitespace(c))
                {
                    continue;
                }
                else if((c == end_characters) || ...)
                {
                    return c;
                }
                else
                {
                    return json_parse_error(ifs);
                }
            }
        }

        template<template<class> class Container>
        inline std::variant<JsonStruct, std::runtime_error> parse_struct(std::ifstream& ifs)
        {
            Container<JsonObject> ret{};

            while(true)
            {
                char c{-1};
                ifs >> c;
                
                if(is_whitespace(c))
                {
                    continue;
                }
                else if(c == '\"')
                {
                    auto&& identifier = parse_string(ifs);
                    if(const auto p = std::get_if<std::runtime_error>(identifier); p)
                    {
                        return *p;
                    }

                    if(const auto p = std::get_if<std::runtime_error>(parse_whitespace(ifs, ':')); p)
                    {
                        return *p;
                    }

                    auto&& value = parse_value(ifs);
                    if(const auto p = std::get_if<std::runtime_error>(value); p)
                    {
                        return *p;
                    }

                    ret.emplace_back(&ret, identifier, value);

                    auto&& end_character = parse_whitespace(ifs, ',', '}');
                    if(const auto p = std::get_if<std::runtime_error>(end_character); p)
                    {
                        return *p;
                    }
                    else if(std:get<char>(end_character) == ',')
                    {
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    return json_parse_error(ifs);
                }
            }

            return ret;
        }
    }

    inline JsonObject json_parse(const std::string_view filename)
    {
        JsonObject ret{nullptr, filename};
        
    }
}