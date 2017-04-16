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

#ifndef AQUARIUS_CXX_JSON2_JSON_PARSER_HPP
#define AQUARIUS_CXX_JSON2_JSON_PARSER_HPP

#include <cmath>

#include <aquarius.hpp>

#include "json.hpp"

namespace json {

struct ToNumber {
    std::unique_ptr<JSONNumber> operator()(std::string &&str) const {
        return make_unique<JSONNumber>(std::stod(str));
    }
};

struct AppendToObject {
    void operator()(std::unique_ptr<JSONObject> &json,
                    std::unique_ptr<JSONString> &&l, std::unique_ptr<JSON> &&r) const {
        json->value().insert(std::make_pair(std::move(l), std::move(r)));
    }
};

struct AppendToArray {
    void operator()(std::unique_ptr<JSONArray> &array, std::unique_ptr<JSON> &&v) const {
        array->value().push_back(std::move(v));
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
constexpr auto string = text[ ch('"') >> *(escape | !set("\"\\") >> ANY) >> ch('"') ] >> construct<JSONString *>();

constexpr auto integer = ch('0') | set("1-9") >> *set("0-9");
constexpr auto exp = set("eE") >> -set("+-") >> integer;
constexpr auto number = text[ -ch('-') >> integer >> ch('.') >> +set("0-9") >> -exp
                                | -ch('-') >> integer ] >> map<ToNumber>();

AQUARIUS_DECL_RULE(std::unique_ptr<JSONObject>, object);
AQUARIUS_DECL_RULE(std::unique_ptr<JSONArray>, array);

AQUARIUS_DEFINE_RULE(
        std::unique_ptr<JSON>, value,
        (string >> cast<JSON>()
         | number
         | nterm<object>()
         | nterm<array>()
         | str("true") >> supply(true) >> construct<JSONBool *>()
         | str("false") >> supply(false) >> construct<JSONBool *>()
         | str("null") >> construct<JSONNull *>()
        ) >> space
);

constexpr auto keyValue = string >> kvSep >> nterm<value>() >> space;

AQUARIUS_DEFINE_RULE(
        std::unique_ptr<JSONArray>, array,
        arrayOpen >> construct<JSONArray *>() >>
                join_each0<AppendToArray>(nterm<value>(), vSep) >> arrayClose
);

AQUARIUS_DEFINE_RULE(
        std::unique_ptr<JSONObject>, object,
        objectOpen >> construct<JSONObject *>() >>
                join_each0<AppendToObject>(keyValue, vSep) >> objectClose
);

AQUARIUS_DEFINE_RULE(
        std::unique_ptr<JSON>, json,
        space >> (nterm<object>() >> cast<JSON>() | nterm<array>())
);

} // namespace json


#endif //AQUARIUS_CXX_JSON2_JSON_PARSER_HPP
