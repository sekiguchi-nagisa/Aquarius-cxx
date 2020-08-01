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

constexpr auto space = *set(" \t\r\n");

constexpr auto objectOpen = ch('{') >> space;
constexpr auto objectClose = ch('}') >> space;

constexpr auto arrayOpen = ch('[') >> space;
constexpr auto arrayClose = ch(']') >> space;

constexpr auto kvSep = space >> ch(':') >> space;
constexpr auto vSep = ch(',') >> space;

constexpr auto escape = ch('\\') >> set("\"\\/bfnrt");
constexpr auto string = text[ ch('"') >> *(escape | !set("\"\\") >> ANY) >> ch('"') ];

constexpr auto integer = ch('0') | set("1-9") >> *set("0-9");
constexpr auto exp = set("eE") >> -set("+-") >> integer;
constexpr auto number = text[ -ch('-') >> integer >> ch('.') >> +set("0-9") >> -exp
                        | -ch('-') >> integer ] >> map<ToNumber>();

AQ_DECL_RULE(object, JSON);
AQ_DECL_RULE(array, JSON);

AQ_DEFINE_RULE(value, JSON) {
    return (string >> construct<JSON>()
            | number
            | nterm<object>()
            | nterm<array>()
            | str("true") >> supply(true) >> construct<JSON>()
            | str("false") >> supply(false) >> construct<JSON>()
            | str("null") >> construct<JSON>()
            ) >> space;
}

constexpr auto keyValue = string >> kvSep >> nterm<value>() >> space;

AQ_DEFINE_RULE(array, JSON) {
    return arrayOpen >> repeat<0>(nterm<value>(), vSep) >> arrayClose >> construct<JSON>();
}

AQ_DEFINE_RULE(object, JSON) {
    return objectOpen >> map<NewObject>()
            >> join_each0<AppendToMap>(keyValue, vSep) >> objectClose >> construct<JSON>();
}

AQ_DEFINE_RULE(json, JSON) {
    return space >> (nterm<object>() | nterm<array>());
}

} // namespace json


#endif //AQUARIUS_CXX_JSON3_JSON_PARSER_HPP
