/*
 * Copyright (C) 2016 Nagisa Sekiguchi
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AQUARIUS_CXX_JSON3_JSON_PARSER_HPP
#define AQUARIUS_CXX_JSON3_JSON_PARSER_HPP

#include <aquarius.hpp>

#include "json.hpp"

namespace json {

struct ToNumber {
    JSON operator()(std::string &&str) const {
        return JSON(std::stod(str));
    }
};

struct NewObject {
    std::map<std::string, JSON> operator()() const {
        return std::map<std::string, JSON>();
    }
};

struct AppendToMap {
    void operator()(std::map<std::string, JSON> &map, std::string &&key, JSON &&value) const {
        map.insert(std::make_pair(std::move(key), std::move(value)));
    }
};

using namespace aquarius;
using namespace aquarius::ascii;

constexpr auto space = *" \t\r\n"_set;

constexpr auto objectOpen = '{'_ch >> space;
constexpr auto objectClose = '}'_ch >> space;

constexpr auto arrayOpen = '['_ch >> space;
constexpr auto arrayClose = ']'_ch >> space;

constexpr auto kvSep = space >> ':'_ch >> space;
constexpr auto vSep = ','_ch >> space;

constexpr auto escape = '\\'_ch >> "\"\\/bfnrt"_set;
constexpr auto string = text[ '"'_ch >> *(escape | !"\"\\"_set >> ANY) >> '"'_ch ];

constexpr auto integer = '0'_ch | "1-9"_set >> *"0-9"_set;
constexpr auto exp = "eE"_set >> -"+-"_set >> integer;
constexpr auto number = text[ -'-'_ch >> integer >> '.'_ch >> +"0-9"_set >> -exp
                        | -'-'_ch >> integer ] >> map<ToNumber>();

AQUARIUS_DECL_RULE(JSON, object);
AQUARIUS_DECL_RULE(JSON, array);

AQUARIUS_DEFINE_RULE(
        JSON, value,
        (string >> construct<JSON>()
         | number
         | nterm<object>()
         | nterm<array>()
         | "true"_str >> supply(true) >> construct<JSON>()
         | "false"_str >> supply(false) >> construct<JSON>()
         | "null"_str >> construct<JSON>()
        ) >> space
);

constexpr auto keyValue = string >> kvSep >> nterm<value>() >> space;

AQUARIUS_DEFINE_RULE(
        JSON, array,
        arrayOpen >> repeat<0>(nterm<value>(), vSep) >> arrayClose >> construct<JSON>()
);

AQUARIUS_DEFINE_RULE(
        JSON, object,
        objectOpen >> map<NewObject>()
                   >> join_each0<AppendToMap>(keyValue, vSep) >> objectClose >> construct<JSON>()
);

AQUARIUS_DEFINE_RULE(
        JSON, json,
        space >> (nterm<object>() | nterm<array>())
);

} // namespace json


#endif //AQUARIUS_CXX_JSON3_JSON_PARSER_HPP
