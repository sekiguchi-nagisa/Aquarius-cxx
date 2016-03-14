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
#include "internal/mapper.hpp"

namespace aquarius {

constexpr expression::StringLiteral operator "" _str(const char *text, std::size_t size) {
    return expression::StringLiteral(text, size);
}

constexpr expression::Char operator ""_ch(char ch) {
    return ch >= 0 ? expression::Char(ch) : throw std::logic_error("must be ascii character");
}

template <typename ... T>
constexpr expression::CharClass set(char ch, T ... rest) {
    return expression::CharClass(misc::makeAsciiMap(ch, rest...));
}

template <typename ... T>
constexpr expression::CharClass set(misc::AsciiMap map, T ... rest) {
    return expression::CharClass(misc::makeAsciiMap(map, rest...));
}

constexpr misc::AsciiMap r(char start, char stop) {
    return misc::makeFromRange(start, stop);
}

constexpr expression::Any ANY;

constexpr expression::CaptureHolder text;

template <typename T, misc::enable_when<expression::is_expr<T>::value> = misc::enabler>
constexpr expression::ZeroMore<T> operator*(T expr) {
    return expression::ZeroMore<T>(expr);
}

template <typename T, misc::enable_when<expression::is_expr<T>::value> = misc::enabler>
constexpr expression::OneMore<T> operator+(T expr) {
    return expression::OneMore<T>(expr);
}

template <typename T, misc::enable_when<expression::is_expr<T>::value> = misc::enabler>
constexpr expression::Option<T> operator-(T expr) {
    return expression::Option<T>(expr);
}

template <typename T, misc::enable_when<expression::is_expr<T>::value> = misc::enabler>
constexpr expression::NotPredicate<T> operator!(T expr) {
    return expression::NotPredicate<T>(expr);
}

template <typename T, misc::enable_when<expression::is_expr<T>::value> = misc::enabler>
constexpr auto operator&(T expr) -> decltype(!(!expr)) {
    return !(!expr);
}

template <typename L, typename R,
        misc::enable_when<expression::is_expr<L>::value && expression::is_expr<R>::value> = misc::enabler>
constexpr expression::Sequence<L, R> operator>>(L left, R right) {
    return expression::Sequence<L, R>(left, right);
};

template <typename L, typename R,
        misc::enable_when<expression::is_expr<L>::value && expression::is_expr<R>::value> = misc::enabler>
constexpr expression::Choice<L, R> operator|(L left, R right) {
    return expression::Choice<L, R>(left, right);
};

template <typename T>
struct nterm {
    static constexpr auto v = expression::NonTerminal<T>();
};

template <typename T, typename M,
        misc::enable_when<expression::is_expr<T>::value &&
                expression::is_mapper<M>::value> = misc::enabler>
constexpr expression::MapperAdapter<T, M> operator>>(T expr, M mapper) {
    return expression::MapperAdapter<T, M>(expr, mapper);
}

template <typename F>
constexpr mapper::CommonMapper<F> map() {
    return mapper::CommonMapper<F>();
}

template <typename T>
constexpr mapper::Constructor<T> construct() {
    return mapper::Constructor<T>();
}

template <typename T>
constexpr mapper::Constructor<std::unique_ptr<T>, true> cons_unique() {
    return mapper::Constructor<std::unique_ptr<T>, true>();
}

template <typename T>
constexpr mapper::Supplier<T> supply(T t) {
    return mapper::Supplier<T>(t);
}

template <typename F, typename T>
constexpr mapper::Joiner<F, T> join(T expr) {
    return mapper::Joiner<F, T>(expr);
};

template <typename F, typename T, typename D>
constexpr mapper::EachJoiner0<F, T, D> join_each0(T expr, D delim) {
    return mapper::EachJoiner0<F, T, D>(expr, delim);
};

template <typename F, typename T>
constexpr mapper::EachJoiner0<F, T, expression::Empty> join_each0(T expr) {
    return mapper::EachJoiner0<F, T, expression::Empty>(expr, expression::Empty());
};


template <typename T>
class ParsedResult {
private:
    Optional<T> value_;

public:
    ParsedResult() = default;
    ParsedResult(T &&value) : value_(std::move(value)) { }

    ParsedResult(ParsedResult &&r) : value_(std::move(r.value_)) { }

    ~ParsedResult() = default;

    ParsedResult<T> &operator=(ParsedResult<T> &&r) {
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

template <typename RULE>
struct Parser {
    using retType = typename decltype(nterm<RULE>::v)::retType;

    template <typename RandomAccessIterator>
    ParsedResult<retType> operator()(RandomAccessIterator begin, RandomAccessIterator end) const {
        constexpr auto p = RULE::pattern();

        ParsedResult<retType> r;
        auto state = createState(begin, end);
        auto v = p(state);
        if(state.result()) {
            r = ParsedResult<retType>(std::move(v));
        }
        return std::move(r);
    }
};

} // namespace aquarius

// helper macro

#define AQUARIUS_ASSERT_PATTERN(P) \
static_assert(std::is_same<T, decltype(P)::retType>::value, "must be same type")

#define AQUARIUS_DEFINE_RULE(R, name, p) \
template <typename T> \
struct name ## __impl { \
    using name = name ## __impl<R>;\
    AQUARIUS_ASSERT_PATTERN(p);\
    static constexpr auto pattern() -> decltype(p) { return p; }\
}; using name = name ## __impl<R>

#define AQUARIUS_DECL_RULE(R, name) \
template <typename T> struct name ## __impl; using name = name ## __impl<R>


#endif //AQUARIUS_CXX_AQUARIUS_HPP
