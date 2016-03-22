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
                    std::tuple<std::unique_ptr<JSONString>, std::unique_ptr<JSON>> &&t) const {
        json->value().insert(std::make_pair(std::get<0>(std::move(t)), std::get<1>(std::move(t))));
    }
};

struct AppendToArray {
    void operator()(std::unique_ptr<JSONArray> &array, std::unique_ptr<JSON> &&v) const {
        array->value().push_back(std::move(v));
    }
};


using namespace aquarius;

constexpr auto space = *" \t\r\n"_set;

constexpr auto objectOpen = '{'_ch >> space;
constexpr auto objectClose = '}'_ch >> space;

constexpr auto arrayOpen = '['_ch >> space;
constexpr auto arrayClose = ']'_ch >> space;

constexpr auto kvSep = space >> ':'_ch >> space;
constexpr auto vSep = ','_ch >> space;

constexpr auto escape = '\\'_ch >> "\"\\/bfnrt"_set;
constexpr auto string = text[ '"'_ch >> *(escape | !"\"\\"_set >> ANY) >> '"'_ch ] >> construct<JSONString *>();

constexpr auto integer = '0'_ch | "1-9"_set >> *"0-9"_set;
constexpr auto exp = "eE"_set >> -"+-"_set >> integer;
constexpr auto number = text [ -'-'_ch >> integer >> '.'_ch >> +"0-9"_set >> -exp
                                | -'-'_ch >> integer ] >> map<ToNumber>();

AQUARIUS_DECL_RULE(std::unique_ptr<JSONObject>, object);
AQUARIUS_DECL_RULE(std::unique_ptr<JSONArray>, array);

AQUARIUS_DEFINE_RULE(
        std::unique_ptr<JSON>, value,
        (string >> cast<JSON>()
         | number
         | nterm<object>::v
         | nterm<array>::v
         | "true"_str >> supply(true) >> construct<JSONBool *>()
         | "false"_str >> supply(false) >> construct<JSONBool *>()
         | "null"_str >> construct<JSONNull *>()
        ) >> space
);

constexpr auto keyValue = string >> kvSep >> nterm<value>::v >> space;

AQUARIUS_DEFINE_RULE(
        std::unique_ptr<JSONArray>, array,
        arrayOpen >> construct<JSONArray *>() >>
                join_each0<AppendToArray>(nterm<value>::v, vSep) >> arrayClose
);

AQUARIUS_DEFINE_RULE(
        std::unique_ptr<JSONObject>, object,
        objectOpen >> construct<JSONObject *>() >>
                join_each0<AppendToObject>(keyValue, vSep) >> objectClose
);

AQUARIUS_DEFINE_RULE(
        std::unique_ptr<JSON>, json,
        space >> (nterm<object>::v >> cast<JSON>() | nterm<array>::v)
);

} // namespace json


#endif //AQUARIUS_CXX_JSON2_JSON_PARSER_HPP
