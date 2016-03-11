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

constexpr auto space = *ch(' ', '\t', '\r', '\n');

constexpr auto objectOpen = ch('{') >> space;
constexpr auto objectClose = ch('}') >> space;

constexpr auto arrayOpen = ch('[') >> space;
constexpr auto arrayClose = ch(']') >> space;

constexpr auto kvSep = space >> ch(':') >> space;
constexpr auto vSep = ch(',') >> space;

constexpr auto escape = ch('\\') >> ch('"', '\\', '/', 'b', 'f', 'n', 'r', 't');
constexpr auto string = ch('"') >> *(escape | !ch('"', '\\') >> ANY) >> ch('"');

constexpr auto integer = ch('0') | ch(r('1', '9')) >> *ch(r('0', '9'));
constexpr auto exp = ch('E', 'e') >> -ch('+', '-') >> integer;
constexpr auto number = -ch('-') >> integer >> ch('.') >> +(ch(r('0', '9'))) >> -exp
                        | -ch('-') >> integer;

AQUARIUS_DECL_RULE(unit, object);
AQUARIUS_DECL_RULE(unit, array);

AQUARIUS_DEFINE_RULE(
        unit, value,
        (string | number | nterm<object>() | nterm<array>() | str("true") | str("false") | str("null")) >> space
);

AQUARIUS_DEFINE_RULE(
        unit, keyValue,
        string >> kvSep >> nterm<value>() >> space
);

AQUARIUS_DEFINE_RULE(
        unit, array,
        arrayOpen >> -(nterm<value>() >> *(vSep >> nterm<value>())) >> arrayClose
);

AQUARIUS_DEFINE_RULE(
        unit, object,
        objectOpen >> -(nterm<keyValue>() >> *(vSep >> nterm<keyValue>())) >> objectClose
);

AQUARIUS_DEFINE_RULE(unit, json,
                     space >> (nterm<object>() | nterm<array>())
);

} // namespace json


#endif //AQUARIUS_CXX_JSON_SIMPLE_JSON_PARSER_HPP
