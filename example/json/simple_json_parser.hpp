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

constexpr auto space = *set(" \t\r\n");

constexpr auto objectOpen = ch('{') >> space;
constexpr auto objectClose = ch('}') >> space;

constexpr auto arrayOpen = ch('[') >> space;
constexpr auto arrayClose = ch(']') >> space;

constexpr auto kvSep = space >> ch(':') >> space;
constexpr auto vSep = ch(',') >> space;

constexpr auto escape = ch('\\') >> set("\"\\/bfnrt");
constexpr auto string = ch('"') >> *(escape | !set("\"\\") >> ANY) >> ch('"');

constexpr auto integer = ch('0') | set("1-9") >> *set("0-9");
constexpr auto exp = set("eE") >> -set("+-") >> integer;
constexpr auto number = -ch('-') >> integer >> ch('.') >> +set("0-9") >> -exp
                        | -ch('-') >> integer;

AQ_DECL_RULE(object, void);
AQ_DECL_RULE(array, void);

AQ_DEFINE_RULE(value, void) {
    return (string | number | nterm<object>() | nterm<array>() | str("true") | str("false") | str("null")) >> space;
}

AQ_DEFINE_RULE(keyValue, void) {
    return string >> kvSep >> nterm<value>() >> space;
}

AQ_DEFINE_RULE(array, void) {
    return arrayOpen >> -(nterm<value>() >> *(vSep >> nterm<value>())) >> arrayClose;
}

AQ_DEFINE_RULE(object, void) {
    return objectOpen >> -(nterm<keyValue>() >> *(vSep >> nterm<keyValue>())) >> objectClose;
}

AQ_DEFINE_RULE(json, void) {
    return space >> (nterm<object>() | nterm<array>());
}

} // namespace json


#endif //AQUARIUS_CXX_JSON_SIMPLE_JSON_PARSER_HPP
