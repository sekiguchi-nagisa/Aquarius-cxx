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

constexpr expression::CharClass operator ""_set(const char *text, std::size_t size) {
    return expression::CharClass(ascii::convertToAsciiMap(text, size + 1));
}

constexpr expression::Empty EMPTY;
constexpr expression::Any ANY;

constexpr expression::CaptureHolder text;

template <size_t Low = 0, size_t High = static_cast<size_t>(-1), typename T, typename D>
constexpr auto repeat(T expr, D delim) -> decltype(expression::repeatHelper<Low, High>(expr, delim)) {
    return expression::repeatHelper<Low, High>(expr, delim);
}

template <size_t Low = 0, size_t High = static_cast<size_t>(-1), typename T>
constexpr auto repeat(T expr) -> decltype(repeat<Low, High>(expr, expression::Empty())) {
    return repeat<Low, High>(expr, expression::Empty());
}

template <typename T, misc::enable_when<expression::is_expr<T>::value> = misc::enabler>
constexpr auto operator*(T expr) -> decltype(repeat(expr)) {
    return repeat(expr);
}

template <typename T, misc::enable_when<expression::is_expr<T>::value> = misc::enabler>
constexpr auto operator+(T expr) -> decltype(repeat<1>(expr)) {
    return repeat<1>(expr);
}

template <typename T, misc::enable_when<expression::is_expr<T>::value> = misc::enabler>
constexpr auto operator-(T expr) -> decltype(expression::optionHelper(expr)) {
    return expression::optionHelper(expr);
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
constexpr auto operator>>(L left, R right) -> decltype(expression::seqHelper(left, right)) {
    return expression::seqHelper(left, right);
}

template <typename L, typename R,
        misc::enable_when<expression::is_expr<L>::value && expression::is_expr<R>::value> = misc::enabler>
constexpr auto operator|(L left, R right) -> decltype(expression::choiceHelper(left, right)) {
    return expression::choiceHelper(left, right);
}

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
constexpr mapper::Supplier<T> supply(T t) {
    return mapper::Supplier<T>(t);
}

template <typename T>
constexpr mapper::NullSupplier<T> supplyNull() {
    return mapper::NullSupplier<T>();
}

template <typename T>
constexpr mapper::Cast<T> cast() {
    return mapper::Cast<T>();
}

template <typename F, typename T>
constexpr mapper::Joiner<F, T> join(T expr) {
    return mapper::Joiner<F, T>(expr);
}

template <typename F, size_t Low = 0, size_t High = static_cast<size_t >(-1), typename T, typename D>
constexpr mapper::EachJoiner<F, T, D, Low, High> join_each(T expr, D delim) {
    return mapper::EachJoiner<F, T, D, Low, High>(expr, delim);
}

template <typename F, size_t Low = 0, size_t High = static_cast<size_t >(-1), typename T>
constexpr auto join_each(T expr) -> decltype(join_each<F, Low, High>(expr, expression::Empty())) {
    return join_each<F, Low, High>(expr, expression::Empty());
}

template <typename F, typename T, typename D>
constexpr auto join_each0(T expr, D delim) -> decltype(join_each<F>(expr, delim)) {
    return join_each<F>(expr, delim);
}

template <typename F, typename T>
constexpr auto join_each0(T expr) -> decltype(join_each0<F>(expr, expression::Empty())) {
    return join_each0<F>(expr, expression::Empty());
}


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
    using retType = typename decltype(nterm<RULE>::v)::retType;

    template <typename RandomAccessIterator, typename P = retType,
            misc::enable_when<std::is_void<P>::value> = misc::enabler>
    ParsedResult<void> operator()(RandomAccessIterator begin, RandomAccessIterator end) const {
        constexpr auto p = RULE::pattern();

        ParsedResult<void> r;
        auto state = createState(begin, end);
        p(state);
        if(state.result()) {
            r = ParsedResult<void>(true);
        }
        return std::move(r);
    }

    template <typename RandomAccessIterator, typename P = retType,
            misc::enable_when<!std::is_void<P>::value> = misc::enabler>
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
