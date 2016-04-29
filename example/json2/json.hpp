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

#ifndef AQUARIUS_CXX_JSON2_JSON_H
#define AQUARIUS_CXX_JSON2_JSON_H

#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <memory>

namespace json {

enum class JSONKind {
    NIL,
    BOOL,
    NUMBER,
    STRING,
    ARRAY,
    OBJECT,
};

class JSON {
private:
    JSONKind kind_;

protected:
    JSON(JSONKind kind) : kind_(kind) { }

public:
    virtual ~JSON() = default;

    JSONKind kind() const {
        return this->kind_;
    }

    bool is(JSONKind kind) const {
        return this->kind_ == kind;
    }
};

class JSONNull : public JSON {
public:
    JSONNull() : JSON(JSONKind::NIL) {}
    ~JSONNull() = default;
};

class JSONBool : public JSON {
private:
    bool value_;

public:
    JSONBool(bool value) : JSON(JSONKind::BOOL), value_(value) { }

    ~JSONBool() = default;

    bool value() const {
        return this->value_;
    }
};

class JSONString : public JSON {
private:
    std::string value_;

public:
    JSONString(std::string &&value) :
            JSON(JSONKind::STRING), value_(std::move(value)) { }

    ~JSONString() = default;

    const std::string &value() const {
        return this->value_;
    }
};

class JSONNumber : public JSON {
private:
    double value_;

public:
    JSONNumber(double value) :
            JSON(JSONKind::NUMBER), value_(value) { }

    ~JSONNumber() = default;

    double value() const {
        return this->value_;
    }
};

class JSONArray : public JSON {
private:
    std::vector<std::unique_ptr<JSON>> values_;

public:
    JSONArray() : JSON(JSONKind::ARRAY), values_() { }

    ~JSONArray() = default;

    std::vector<std::unique_ptr<JSON>> &value() {
        return this->values_;
    }

    const std::vector<std::unique_ptr<JSON>> &value() const {
        return this->values_;
    }
};

template <typename T, typename ... A>
inline std::unique_ptr<T> make_unique(A && ...arg) {
    return std::unique_ptr<T>(new T(std::forward<A>(arg)...));
}

struct KeyComparator {
    bool operator()(const std::unique_ptr<JSONString> &x, const std::unique_ptr<JSONString> &y) const {
        return x->value() == y->value();
    }
};

struct Hash {
    std::size_t operator()(const std::unique_ptr<JSONString> &x) const {
        return std::hash<std::string>()(x->value());
    }
};

class JSONObject : public JSON {
public:
    using map_type = std::unordered_map<std::unique_ptr<JSONString>, std::unique_ptr<JSON>, Hash, KeyComparator>;

private:
    map_type values_;

public:
    JSONObject() : JSON(JSONKind::OBJECT), values_() { }

    ~JSONObject() = default;

    map_type &value() {
        return this->values_;
    }

    const map_type  &value() const {
        return this->values_;
    }
};



} // namespace json

#endif //AQUARIUS_CXX_JSON2_JSON_H
