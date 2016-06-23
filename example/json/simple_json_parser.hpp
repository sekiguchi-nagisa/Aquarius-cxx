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

#ifndef AQUARIUS_CXX_JSON_SIMPLE_JSON_PARSER_HPP
#define AQUARIUS_CXX_JSON_SIMPLE_JSON_PARSER_HPP

#include <aquarius.hpp>

namespace json {

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
constexpr auto string = '"'_ch >> *(escape | !"\"\\"_set >> ANY) >> '"'_ch;

constexpr auto integer = '0'_ch | "1-9"_set >> *"0-9"_set;
constexpr auto exp = "eE"_set >> -"+-"_set >> integer;
constexpr auto number = -'-'_ch >> integer >> '.'_ch >> +"0-9"_set >> -exp
                        | -'-'_ch >> integer;

AQUARIUS_DECL_RULE(void, object);
AQUARIUS_DECL_RULE(void, array);

AQUARIUS_DEFINE_RULE(
        void, value,
        (string | number | nterm<object>() | nterm<array>() | "true"_str | "false"_str | "null"_str) >> space
);

AQUARIUS_DEFINE_RULE(
        void, keyValue,
        string >> kvSep >> nterm<value>() >> space
);

AQUARIUS_DEFINE_RULE(
        void, array,
        arrayOpen >> -(nterm<value>() >> *(vSep >> nterm<value>())) >> arrayClose
);

AQUARIUS_DEFINE_RULE(
        void, object,
        objectOpen >> -(nterm<keyValue>() >> *(vSep >> nterm<keyValue>())) >> objectClose
);

AQUARIUS_DEFINE_RULE(
        void, json,
        space >> (nterm<object>() | nterm<array>())
);

} // namespace json


#endif //AQUARIUS_CXX_JSON_SIMPLE_JSON_PARSER_HPP
