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
};

template <typename L, typename R, enable_when<is_tuple<L>::value && !is_tuple<R>::value> = enabler>
inline auto catAsTuple(L &&l, R &&r) -> decltype(std::tuple_cat(std::move(l), std::make_tuple(std::move(r)))) {
    return std::tuple_cat(std::move(l), std::make_tuple(std::move(r)));
};

template <typename L, typename R, enable_when<!is_tuple<L>::value && is_tuple<R>::value> = enabler>
inline auto catAsTuple(L &&l, R &&r) -> decltype(std::tuple_cat(std::make_tuple(std::move(l)), std::move(r))) {
    return std::tuple_cat(std::make_tuple(std::move(l)), std::move(r));
};

template <typename L, typename R, enable_when<is_tuple<L>::value && is_tuple<R>::value> = enabler>
inline auto catAsTuple(L &&l, R &&r) -> decltype(std::tuple_cat(std::move(l), std::move(r))) {
    return std::tuple_cat(std::move(l), std::move(r));
};

/**
 * for tuple unpacking
 */
template <typename Func, typename ... A, typename ... Arg,
        enable_when<(sizeof...(A) == sizeof...(Arg))> = enabler>
inline auto unpackAndApplyImpl(Func &func, std::tuple<A ...> &&tuple, Arg&& ...arg) -> decltype(func(A() ...)) {
    return func(std::forward<Arg>(arg)...);
}

template <typename Func, typename ... A, typename ... Arg,
        enable_when<(sizeof...(A) > sizeof...(Arg))> = enabler>
inline auto unpackAndApplyImpl(Func &func, std::tuple<A ...> &&tuple, Arg&& ...arg) -> decltype(func(A() ...)) {
    return unpackAndApplyImpl(func, std::move(tuple),
                              std::forward<Arg>(arg)..., std::move(std::get<sizeof...(Arg)>(tuple)));
}


template <typename Func, typename ... A>
inline auto unpackAndApply(Func &func, std::tuple<A ...> &&tuple) -> decltype(func(A() ...)) {
    return unpackAndApplyImpl(func, std::move(tuple));
};

template <typename Func, typename A>
inline auto unpackAndApply(Func &func, A &&arg) -> decltype(func(std::forward<A>(arg))) {
    return func(std::forward<A>(arg));
};

template <typename Func>
inline auto unpackAndApply(Func &func, unit &&arg) -> decltype(func()) {
    return func();
}


} // namespace misc
} // namespace aquarius

#endif //AQUARIUS_CXX_INTERNAL_TUPLES_HPP
