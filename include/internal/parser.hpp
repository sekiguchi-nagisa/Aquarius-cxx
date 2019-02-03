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

#ifndef AQUARIUS_CXX_INTERNAL_PARSER_HPP
#define AQUARIUS_CXX_INTERNAL_PARSER_HPP

#include "misc.hpp"
#include "expression.hpp"

namespace aquarius {

template <typename T>
class ParsedResult {
private:
    Optional<T> value_;

public:
    ParsedResult() = default;
    explicit ParsedResult(T &&value) : value_(std::move(value)) { }

    ParsedResult(ParsedResult &&r) noexcept : value_(std::move(r.value_)) { }

    ~ParsedResult() = default;

    ParsedResult<T> &operator=(ParsedResult<T> &&r) noexcept {
        auto tmp(std::move(r));
        this->swap(tmp);
        return *this;
    }

    void swap(ParsedResult<T> &r) {
        this->value_.swap(r.value_);
    }

    explicit operator bool() const {
        return static_cast<bool>(this->value_);
    }

    T &get() {
        return this->value_.get();
    }

    static T extract(ParsedResult<T> &&r) {
        T t = std::move(r.get());
        return t;
    }
};

template <>
class ParsedResult<void> {
private:
    bool success;

public:
    ParsedResult() : success(false) { }

    explicit ParsedResult(bool success) : success(success) { }

    ~ParsedResult() = default;

    explicit operator bool() const {
        return this->success;
    }
};

template <typename RULE>
struct Parser {
    using retType = typename expression::NonTerminal<RULE>::retType;

    template <typename RandomAccessIterator, typename P = retType,
            misc::enable_when<std::is_void<P>::value> = nullptr>
    ParsedResult<void> operator()(RandomAccessIterator begin, RandomAccessIterator end) const {
        constexpr auto p = RULE::pattern;

        ParsedResult<void> r;
        auto state = createState(begin, end);
        p(state);
        if(state.result()) {
            r = ParsedResult<void>(true);
        }
        return r;
    }

    template <typename RandomAccessIterator, typename P = retType,
            misc::enable_when<!std::is_void<P>::value> = nullptr>
    ParsedResult<retType> operator()(RandomAccessIterator begin, RandomAccessIterator end) const {
        constexpr auto p = RULE::pattern;

        ParsedResult<retType> r;
        auto state = createState(begin, end);
        auto v = p(state);
        if(state.result()) {
            r = ParsedResult<retType>(std::move(v));
        }
        return r;
    }
};

} // namespace aquarius

#endif //AQUARIUS_CXX_INTERNAL_PARSER_HPP
