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

constexpr auto space = *set(' ', '\t', '\r', '\n');

constexpr auto objectOpen = '{'_ch >> space;
constexpr auto objectClose = '}'_ch >> space;

constexpr auto arrayOpen = '['_ch >> space;
constexpr auto arrayClose = ']'_ch >> space;

constexpr auto kvSep = space >> ':'_ch >> space;
constexpr auto vSep = ','_ch >> space;

constexpr auto escape = '\\'_ch >> set('"', '\\', '/', 'b', 'f', 'n', 'r', 't');
constexpr auto string = '"'_ch >> *(escape | !set('"', '\\') >> ANY) >> '"'_ch;

constexpr auto integer = '0'_ch | set(r('1', '9')) >> *set(r('0', '9'));
constexpr auto exp = set('E', 'e') >> -set('+', '-') >> integer;
constexpr auto number = -'-'_ch >> integer >> '.'_ch >> +(set(r('0', '9'))) >> -exp
                        | -'-'_ch >> integer;

AQUARIUS_DECL_RULE(unit, object);
AQUARIUS_DECL_RULE(unit, array);

AQUARIUS_DEFINE_RULE(
        unit, value,
        (string | number | nterm<object>::v | nterm<array>::v | "true"_str | "false"_str | "null"_str) >> space
);

AQUARIUS_DEFINE_RULE(
        unit, keyValue,
        string >> kvSep >> nterm<value>::v >> space
);

AQUARIUS_DEFINE_RULE(
        unit, array,
        arrayOpen >> -(nterm<value>::v >> *(vSep >> nterm<value>::v)) >> arrayClose
);

AQUARIUS_DEFINE_RULE(
        unit, object,
        objectOpen >> -(nterm<keyValue>::v >> *(vSep >> nterm<keyValue>::v)) >> objectClose
);

AQUARIUS_DEFINE_RULE(unit, json,
                     space >> (nterm<object>::v | nterm<array>::v)
);

} // namespace json


#endif //AQUARIUS_CXX_JSON_SIMPLE_JSON_PARSER_HPP
