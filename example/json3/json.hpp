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

#ifndef AQUARIUS_CXX_JSON3_JSON_HPP
#define AQUARIUS_CXX_JSON3_JSON_HPP

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <cassert>

namespace json {

class JSON;

class JSON {
public:
    enum JSONKind {
        NIL,
        NUMBER,
        BOOL,
        STRING,
        ARRAY,
        OBJECT,
    };

    using array_type = std::vector<JSON>;
    using map_type = std::map<std::string, JSON>;

private:
    JSONKind kind;

    union {
        void *nil;
        double num;
        bool b;
        std::string str;
        array_type array;
        map_type map;
    };

    void assign(JSON &&json) {
        this->kind = json.kind;
        switch(this->kind) {
        case NIL:
            this->nil = nullptr;
            break;
        case NUMBER:
            this->num = json.num;
            break;
        case BOOL:
            this->b = json.b;
            break;
        case STRING:
            new (&this->str) std::string(std::move(json.str));
            break;
        case ARRAY:
            new (&this->array) array_type(std::move(json.array));
            break;
        case OBJECT:
            new (&this->map) map_type(std::move(json.map));
            break;
        }
    }

    void clear() {
        switch(this->kind) {
        case NIL:
        case NUMBER:
        case BOOL:
            break;
        case STRING:
            this->str.~basic_string();
            break;
        case ARRAY:
            this->array.~vector();
            break;
        case OBJECT:
            this->map.~map();
            break;
        }
    }

public:
    JSON() : kind(NIL), nil(nullptr) {}
    JSON(double num) : kind(NUMBER), num(num) {}
    JSON(bool b) : kind(BOOL), b(b) {}
    JSON(std::string &&str) : kind(STRING), str(std::move(str)) {}
    JSON(array_type &&array) : kind(ARRAY), array(std::move(array)) {}
    JSON(map_type &&map) : kind(OBJECT), map(std::move(map)) {}

    JSON(JSON &&json) { this->assign(std::move(json)); }

    ~JSON() { this->clear(); }

    JSON &operator=(JSON &&json) {
        this->clear();
        this->assign(std::move(json));
        return *this;
    }

    void swap(JSON &json) {
        JSON tmp(std::move(*this));
        *this = std::move(json);
        json = std::move(tmp);
    }

    JSONKind getKind() const {
        return this->kind;
    }

    bool is(JSONKind kind) const {
        return this->getKind() == kind;
    }

    double &asNumber() {
        assert(this->is(NUMBER));
        return this->num;
    }

    bool &asBool() {
        assert(this->is(BOOL));
        return this->b;
    }

    std::string &asString() {
        assert(this->is(STRING));
        return this->str;
    }

    array_type &asArray() {
        assert(this->is(ARRAY));
        return this->array;
    }

    map_type &asObject() {
        assert(this->is(OBJECT));
        return this->map;
    }
};


} // namespace json

#endif //AQUARIUS_CXX_JSON3_JSON_HPP
