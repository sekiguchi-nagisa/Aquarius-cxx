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
struct isTuple : std::false_type {};

template <typename ... A>
struct isTuple<std::tuple<A ...>> : std::true_type {};

template <typename L, typename R, enable_when<!isTuple<L>::value && !isTuple<R>::value> = enabler>
inline auto appendToTuple(L &&l, R &&r) -> decltype(std::make_tuple(std::move(l), std::move(r))) {
    return std::make_tuple(std::move(l), std::move(r));
};

template <typename L, typename R, enable_when<isTuple<L>::value && !isTuple<R>::value> = enabler>
inline auto appendToTuple(L &&l, R &&r) -> decltype(std::tuple_cat(std::move(l), std::make_tuple(std::move(r)))) {
    return std::tuple_cat(std::move(l), std::make_tuple(std::move(r)));
};

template <typename L, typename R, enable_when<!isTuple<L>::value && isTuple<R>::value> = enabler>
inline auto appendToTuple(L &&l, R &&r) -> decltype(std::tuple_cat(std::make_tuple(std::move(l)), std::move(r))) {
    return std::tuple_cat(std::make_tuple(std::move(l)), std::move(r));
};

template <typename L, typename R, enable_when<isTuple<L>::value && isTuple<R>::value> = enabler>
inline auto appendToTuple(L &&l, R &&r) -> decltype(std::tuple_cat(std::move(l), std::move(r))) {
    return std::tuple_cat(std::move(l), std::move(r));
};


} // namespace misc
} // namespace aquarius

#endif //AQUARIUS_CXX_INTERNAL_TUPLES_HPP
