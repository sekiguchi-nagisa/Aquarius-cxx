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
struct is_tuple : std::false_type {};

template <typename ... A>
struct is_tuple<std::tuple<A ...>> : std::true_type {};

template <typename ... A>
struct is_tuple<std::tuple<A ...> &> : std::true_type {};

template <typename ... A>
struct is_tuple<const std::tuple<A ...> &> : std::true_type {};

/**
 * for tuple concatenation
 */
template <typename L, typename R, enable_when<!is_tuple<L>::value && !is_tuple<R>::value> = enabler>
inline auto catAsTuple(L &&l, R &&r) -> decltype(std::make_tuple(std::move(l), std::move(r))) {
    return std::make_tuple(std::move(l), std::move(r));
}

template <typename L, typename R, enable_when<is_tuple<L>::value && !is_tuple<R>::value> = enabler>
inline auto catAsTuple(L &&l, R &&r) -> decltype(std::tuple_cat(std::move(l), std::make_tuple(std::move(r)))) {
    return std::tuple_cat(std::move(l), std::make_tuple(std::move(r)));
}

template <typename L, typename R, enable_when<!is_tuple<L>::value && is_tuple<R>::value> = enabler>
inline auto catAsTuple(L &&l, R &&r) -> decltype(std::tuple_cat(std::make_tuple(std::move(l)), std::move(r))) {
    return std::tuple_cat(std::make_tuple(std::move(l)), std::move(r));
}

template <typename L, typename R, enable_when<is_tuple<L>::value && is_tuple<R>::value> = enabler>
inline auto catAsTuple(L &&l, R &&r) -> decltype(std::tuple_cat(std::move(l), std::move(r))) {
    return std::tuple_cat(std::move(l), std::move(r));
}


#if (__cplusplus >= 201402L)

template <size_t ... N>
using tuple_unpacker = std::index_sequence<N...>;

template <size_t N>
using create_unpacker = typename std::make_index_sequence<N>;

#else

/**
 * for tuple unpacking
 */
namespace __detail_unpacker {

template <size_t ... S>
struct tuple_unpacker { };

template <size_t N, size_t ... S>
struct tuple_unpacker_holder : tuple_unpacker_holder<N - 1, N - 1, S...> { };

template <size_t ... N>
struct tuple_unpacker_holder<0, N...> {
    using type = tuple_unpacker<N...>;
};

} // namespace __detail_unpacker

template <size_t ... N>
using tuple_unpacker = __detail_unpacker::tuple_unpacker<N...>;

template <size_t N>
using create_unpacker = typename __detail_unpacker::tuple_unpacker_holder<N>::type;

#endif


/**
 * apply function with tuple argument.
 */
template <typename Func, typename ... A, size_t ... I>
inline auto unpackAndApplyImpl(std::tuple<A ...> &&tuple, tuple_unpacker<I ...>) -> decltype(Func()(A() ...)) {
    return Func()(std::get<I>(std::move(tuple))...);
}

template <typename Func, typename ... A>
inline auto unpackAndApply(std::tuple<A ...> &&tuple) -> decltype(Func()(A() ...)) {
    return unpackAndApplyImpl<Func>(std::move(tuple), create_unpacker<sizeof...(A)>());
}

template <typename Func, typename A>
inline auto unpackAndApply(A &&arg) -> decltype(Func()(std::forward<A>(arg))) {
    return Func()(std::forward<A>(arg));
}

template <typename Func>
inline auto unpackAndApply() -> decltype(Func()()) {
    return Func()();
}

/**
 * apply function with tuple argument.
 */
template <typename Func, typename T, typename ... A, size_t ... I>
inline void unpackAndAppendImpl(T &recv, std::tuple<A ...> &&tuple, tuple_unpacker<I...>) {
    Func()(recv, std::get<I>(std::move(tuple))...);
}

template <typename Func, typename T, typename ... A>
inline void unpackAndAppend(T &recv, std::tuple<A ...> &&tuple) {
    unpackAndAppendImpl<Func>(recv, std::move(tuple), create_unpacker<sizeof...(A)>());
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
        misc::enable_when<!std::is_pointer<T>::value> = misc::enabler>
inline T construct(Arg && ...arg) {
    return T(std::forward<Arg>(arg)...);
}

template <typename T, typename ... Arg,
        misc::enable_when<std::is_pointer<T>::value> = misc::enabler>
inline auto construct(Arg && ...arg) -> std::unique_ptr<typename std::remove_pointer<T>::type> {
    using P = typename std::remove_pointer<T>::type;
    return std::unique_ptr<P>(new P(std::forward<Arg>(arg)...));
}

template <typename T, typename ... A, size_t ... I>
inline type_of_constructor_t<T> unpackAndConstructImpl(std::tuple<A ...> &&tuple, tuple_unpacker<I...>) {
    return construct<T>(std::get<I>(std::move(tuple))...);
}

template <typename T, typename ... A>
inline type_of_constructor_t<T> unpackAndConstruct(std::tuple<A ...> &&tuple) {
    return unpackAndConstructImpl<T>(std::move(tuple), create_unpacker<sizeof...(A)>());
}

template <typename T, typename A>
inline type_of_constructor_t<T> unpackAndConstruct(A &&arg) {
    return construct<T>(std::forward<A>(arg));
}

template <typename T>
inline type_of_constructor_t<T> unpackAndConstruct() {
    return construct<T>();
}

} // namespace misc
} // namespace aquarius

#endif //AQUARIUS_CXX_INTERNAL_TUPLES_HPP
