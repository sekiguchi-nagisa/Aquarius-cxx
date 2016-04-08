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

#ifndef AQUARIUS_CXX_INTERNAL_COMBINATOR_HPP
#define AQUARIUS_CXX_INTERNAL_COMBINATOR_HPP

#include "expression.hpp"

namespace aquarius {
inline namespace ascii {

constexpr expression::Any ANY;

constexpr expression::StringLiteral operator "" _str(const char *text, std::size_t size) {
    return expression::StringLiteral(text, size);
}

constexpr expression::Char operator ""_ch(char ch) {
    return ch >= 0 ? expression::Char(ch) : throw std::logic_error("must be ascii character");
}

constexpr expression::CharClass operator ""_set(const char *text, std::size_t size) {
    return expression::CharClass(ascii_map::convertToAsciiMap(text, size));
}

template <size_t N>
constexpr expression::CharClass set(const char (&text)[N]) {
    return expression::CharClass(ascii_map::convertToAsciiMap(text, N - 1));
}

} // inline namespace ascii

namespace utf8 {

constexpr expression::Utf8Any ANY;

} // namespace utf8


constexpr expression::Empty EMPTY;

constexpr expression::CaptureHolder text;

template <size_t Low = 0, size_t High = static_cast<size_t>(-1), typename T, typename D>
constexpr auto repeat(T expr, D delim) -> decltype(expression::repeatHelper<Low, High>(expr, delim)) {
    return expression::repeatHelper<Low, High>(expr, delim);
}

template <size_t Low = 0, size_t High = static_cast<size_t>(-1), typename T>
constexpr auto repeat(T expr) -> decltype(repeat<Low, High>(expr, expression::Empty())) {
    return repeat<Low, High>(expr, expression::Empty());
}

template <typename T, misc::enable_when<expression::is_expr<T>::value> = nullptr>
constexpr auto operator*(T expr) -> decltype(repeat(expr)) {
    return repeat(expr);
}

template <typename T, misc::enable_when<expression::is_expr<T>::value> = nullptr>
constexpr auto operator+(T expr) -> decltype(repeat<1>(expr)) {
    return repeat<1>(expr);
}

template <typename T, misc::enable_when<expression::is_expr<T>::value> = nullptr>
constexpr auto operator-(T expr) -> decltype(expression::optionHelper(expr)) {
    return expression::optionHelper(expr);
}

template <typename T, misc::enable_when<expression::is_expr<T>::value> = nullptr>
constexpr expression::NotPredicate<T> operator!(T expr) {
    return expression::NotPredicate<T>(expr);
}

template <typename T, misc::enable_when<expression::is_expr<T>::value> = nullptr>
constexpr auto operator&(T expr) -> decltype(!(!expr)) {
    return !(!expr);
}

template <typename L, typename R,
        misc::enable_when<expression::is_expr<L>::value && expression::is_expr<R>::value> = nullptr>
constexpr auto operator>>(L left, R right) -> decltype(expression::seqHelper(left, right)) {
    return expression::seqHelper(left, right);
}

template <typename L, typename R,
        misc::enable_when<expression::is_expr<L>::value && expression::is_expr<R>::value> = nullptr>
constexpr auto operator|(L left, R right) -> decltype(expression::choiceHelper(left, right)) {
    return expression::choiceHelper(left, right);
}

template <typename T>
constexpr expression::NonTerminal<T> nterm() {
    return expression::NonTerminal<T>();
}

template <typename T, typename M,
        misc::enable_when<expression::is_expr<T>::value &&
                          expression::is_mapper<M>::value> = nullptr>
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

// define C++14 specific operator
#if (__cplusplus >= 201402L)

template <typename T>
constexpr auto nterm_c = expression::NonTerminal<T>();

template <typename T>
constexpr auto map_c = mapper::CommonMapper<T>();

template <typename T>
constexpr auto cons_c = mapper::Constructor<T>();

template <typename T>
constexpr auto cast_c = mapper::Cast<T>();

#endif

} // namespace aquarius

#endif //AQUARIUS_CXX_INTERNAL_COMBINATOR_HPP
