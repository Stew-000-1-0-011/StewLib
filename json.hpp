#pragma once

#include <variant>
#include <vector>
#include <list>
#include <optional>
#include <string>
#include <cstdio>
#include <cstddef>

namespace StewLib
{
    struct JsonObject;
    struct JsonValue;

    namespace Implement::JsonImp
    {
        template<template<class> class Container>
        struct JsonStructBody final
        {
            // 不完全型をサポート until C++17
            Container<JsonObject> json_objects{};
        };

        using JsonStruct = std::variant<JsonStructBody<std::vector>, JsonStructBody<std::list>>;

        template<template<class> class Container>
        struct JsonArrayBody final
        {
            Container<JsonValue> json_values{};
        };

        using JsonArray = std::variant<JsonArrayBody<std::vector>, JsonArrayBody<std::list>>;

        using JsonValueType = std::variant<std::nullopt_t, bool, double, std::string, JsonArray, JsonStruct>;
    }

    struct JsonObject final
    {
        JsonObject * parent;
        std::string identifier;
        Implement::JsonImp::JsonValueType value;

        inline std::string get_path() const
        {
            if(parent) return parent->get_path() + '/' + identifier;
            else return "";
        }
    };

    struct JsonValue final
    {
        Implement::JsonImp::JsonValueType value;
    };

    namespace Implement::JsonImp
    {
        inline std::size_t parsed;
        constexpr unsigned char eof = EOF;

        constexpr struct FileIOError{} file_io_error;
        constexpr struct DontMatch{} dont_match;

        using ParserReturnType = std::variant<FileIOError, DontMatch, std::string>;

        constexpr auto parse_white_space = [](std::FILE *const fp) noexcept -> ParserReturnType
        {
            std::string ret;
            unsigned char c;

            while((c = std::fgetc(fp)) != eof)
            {
                if(c != ' ' && c != '\t' && c != '\n' && c != '\r')
                {
                    if(std::ungetc(c, fp) == EOF)
                    {
                        return file_io_error;
                    }
                    else goto Return;
                }
            }
            if(std::feof(fp))
            {
                return file_io_error;
            }
            
        Return:
            if(ret.length()) return ret;
            else return dont_match;
        };

        constexpr auto parse_string_literal = [](std::FILE *const fp) noexcept -> ParserReturnType
        {
            std::string ret;
            unsigned char c;

            if(unsigned char c = std::fgetc(fp); c != eof)
            {
                
            }
            else if(std::feof(fp))
            {
                return ret;
            }
        };
    }
}