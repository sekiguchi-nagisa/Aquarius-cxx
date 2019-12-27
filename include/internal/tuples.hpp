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

#ifndef AQUARIUS_CXX_INTERNAL_TUPLES_HPP
#define AQUARIUS_CXX_INTERNAL_TUPLES_HPP

#include <tuple>
#include <memory>

#include "misc.hpp"

namespace aquarius {
namespace misc {

/**
 * check whether a type is tuple or not.
 */
template <typename T>
struct is_tuple : is_specialization_of<typename std::remove_reference<
        typename std::remove_const<T>::type>::type, std::tuple> { };


/**
 * for tuple concatenation
 */
template <typename L, typename R, enable_when<!is_tuple<L>::value && !is_tuple<R>::value> = nullptr>
inline auto catAsTuple(L &&l, R &&r) {
    return std::make_tuple(std::forward<L>(l), std::forward<R>(r));
}

template <typename L, typename R, enable_when<is_tuple<L>::value && !is_tuple<R>::value> = nullptr>
inline auto catAsTuple(L &&l, R &&r) {
    return std::tuple_cat(std::forward<L>(l), std::make_tuple(std::forward<R>(r)));
}

template <typename L, typename R, enable_when<!is_tuple<L>::value && is_tuple<R>::value> = nullptr>
inline auto catAsTuple(L &&l, R &&r) {
    return std::tuple_cat(std::make_tuple(std::forward<L>(l)), std::forward<R>(r));
}

template <typename L, typename R, enable_when<is_tuple<L>::value && is_tuple<R>::value> = nullptr>
inline auto catAsTuple(L &&l, R &&r) {
    return std::tuple_cat(std::forward<L>(l), std::forward<R>(r));
}


/**
 * apply function with tuple argument.
 */
template <typename Func, typename ... A, size_t ... I>
inline auto unpackAndApplyImpl(std::tuple<A ...> &&tuple, std::index_sequence<I ...>) {
    return Func()(std::get<I>(std::move(tuple))...);
}

template <typename Func, typename ... A>
inline auto unpackAndApply(std::tuple<A ...> &&tuple) {
    return unpackAndApplyImpl<Func>(std::move(tuple), std::make_index_sequence<sizeof...(A)>());
}

template <typename Func, typename A>
inline auto unpackAndApply(A &&arg) {
    return Func()(std::forward<A>(arg));
}

template <typename Func>
inline auto unpackAndApply() {
    return Func()();
}

/**
 * apply function with tuple argument.
 */
template <typename Func, typename T, typename ... A, size_t ... I>
inline void unpackAndAppendImpl(T &recv, std::tuple<A ...> &&tuple, std::index_sequence<I...>) {
    Func()(recv, std::get<I>(std::move(tuple))...);
}

template <typename Func, typename T, typename ... A>
inline void unpackAndAppend(T &recv, std::tuple<A ...> &&tuple) {
    unpackAndAppendImpl<Func>(recv, std::move(tuple), std::make_index_sequence<sizeof...(A)>());
}

template <typename Func, typename T, typename A>
inline void unpackAndAppend(T &recv, A &&value) {
    Func()(recv, std::forward<A>(value));
}

/**
 * construct object with tuple argument.
 */
template <typename T>
struct type_of_constructor {
    using type = T;
};

template <typename T>
struct type_of_constructor<T *> {
    using type = std::unique_ptr<T>;
};

template <typename T>
using type_of_constructor_t = typename type_of_constructor<T>::type;

template <typename T, typename ... Arg,
        misc::enable_when<!std::is_pointer<T>::value> = nullptr>
inline T construct(Arg && ...arg) {
    return T(std::forward<Arg>(arg)...);
}

template <typename T, typename ... Arg,
        misc::enable_when<std::is_pointer<T>::value> = nullptr>
inline auto construct(Arg && ...arg) {
    using P = typename std::remove_pointer<T>::type;
    return std::unique_ptr<P>(new P(std::forward<Arg>(arg)...));
}

template <typename T, typename ... A, size_t ... I>
inline auto unpackAndConstructImpl(std::tuple<A ...> &&tuple, std::index_sequence<I...>) {
    return construct<T>(std::get<I>(std::move(tuple))...);
}

template <typename T, typename ... A>
inline auto unpackAndConstruct(std::tuple<A ...> &&tuple) {
    return unpackAndConstructImpl<T>(std::move(tuple), std::make_index_sequence<sizeof...(A)>());
}

template <typename T, typename A>
inline auto unpackAndConstruct(A &&arg) {
    return construct<T>(std::forward<A>(arg));
}

template <typename T>
inline auto unpackAndConstruct() {
    return construct<T>();
}

} // namespace misc
} // namespace aquarius

#endif //AQUARIUS_CXX_INTERNAL_TUPLES_HPP
