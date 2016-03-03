/*
 * Copyright (C) 2015-2016 Nagisa Sekiguchi
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

#ifndef AQUARIUS_CXX_AQUARIUS_HPP
#define AQUARIUS_CXX_AQUARIUS_HPP

#include "internal/expression.hpp"

namespace aquarius {

constexpr expression::Char str(const char (&text)[2]) {
    return expression::Char(text[0]);
}

template <std::size_t N>
constexpr expression::StringLiteral str(const char (&text)[N]) {
    return expression::StringLiteral(text, N - 1);
}

constexpr expression::StringLiteral operator "" _str(const char *text, std::size_t size) {
    return expression::StringLiteral(text, size);
}

constexpr expression::Char ch(char ch) {
    return ch >= 0 ? expression::Char(ch) : throw std::logic_error("must be ascii character");
}

template <typename ... T>
constexpr expression::CharClass ch(char ch, T ... rest) {
    return expression::CharClass(misc::makeAsciiMap(ch, rest...));
}

template <typename ... T>
constexpr expression::CharClass ch(misc::AsciiMap map, T ... rest) {
    return expression::CharClass(misc::makeAsciiMap(map, rest...));
}

constexpr misc::AsciiMap r(char start, char stop) {
    return misc::makeFromRange(start, stop);
}

constexpr expression::Any ANY;

constexpr expression::CaptureHolder text;

template <typename T>
constexpr expression::ZeroMore<T> operator*(T expr) {
    return expression::ZeroMore<T>(expr);
}

template <typename T>
constexpr expression::OneMore<T> operator+(T expr) {
    return expression::OneMore<T>(expr);
}

template <typename T>
constexpr expression::AndPredicate<T> operator&(T expr) {
    return expression::AndPredicate<T>(expr);
}

template <typename T>
constexpr expression::NotPredicate<T> operator!(T expr) {
    return expression::NotPredicate<T>(expr);
}

template <typename L, typename R>
constexpr expression::Sequence<L, R> operator>>(L left, R right) {
    return expression::Sequence<L, R>(left, right);
};

template <typename T>
constexpr expression::NonTerminal<T> nterm() {
    return expression::NonTerminal<T>();
}

template <typename T>
struct Rule {
    using retType = T;
};

template <typename T>
class ParsedResult : public misc::NonCopyable<ParsedResult<T>> {
private:
    T *value_;

public:
    ParsedResult() : value_(nullptr) { }
    ParsedResult(T &&value) : value_(new T()) {
        *this->value_ = std::move(value);
    }

    ParsedResult(ParsedResult &&r) : value_(r.value_) {
        r.value_ = nullptr;
    }

    ~ParsedResult() {
        delete this->value_;
    }

    ParsedResult<T> &operator=(ParsedResult<T> &&r) {
        auto tmp(std::move(r));
        this->swap(tmp);
        return *this;
    }

    void swap(ParsedResult<T> &r) {
        std::swap(this->value_, r.value_);
    }

    explicit operator bool() const {
        return this->value_ != nullptr;
    }

    T &get() {
        return *this->value_;
    }

    static T extract(ParsedResult<T> &&r) {
        T v;
        std::swap(v, r.get());
        return v;
    }
};

template <typename RULE>
struct Parser {
    using retType = typename RULE::retType;

    template <typename RandomAccessIterator>
    ParsedResult<retType> operator()(RandomAccessIterator begin, RandomAccessIterator end) const {
        static_assert(misc::isConstant(RULE::pattern()), "must be constant");

        ParsedResult<retType> r;
        auto state = createState(begin, end);
        auto v = RULE::pattern()(state);
        if(state.result()) {
            r = ParsedResult<retType>(std::move(v));
        }
        return std::move(r);
    }
};

} // namespace aquarius

// helper macro

#define AQUARIUS_ASSERT_PATTERN(P) \
static_assert(std::is_same<retType, decltype(P)::retType>::value, "must be same type")

#define AQUARIUS_RHS(t, p) \
Rule<t> {\
    AQUARIUS_ASSERT_PATTERN(p);\
    static constexpr auto pattern() -> decltype(p) { return p; }\
}\

#define AQUARIUS_DEFINE_RULE(t, name, p) struct name : AQUARIUS_RHS(t, p)



#endif //AQUARIUS_CXX_AQUARIUS_HPP
