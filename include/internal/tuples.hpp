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
 * apply function object with tuple argument.
 */
template <typename Func, typename ... A, typename ... Arg,
        enable_when<(sizeof...(A) == sizeof...(Arg))> = enabler>
inline auto unpackAndApplyImpl(std::tuple<A ...> &&tuple, Arg&& ...arg) -> decltype(Func()(A() ...)) {
    return Func()(std::forward<Arg>(arg)...);
}

template <typename Func, typename ... A, typename ... Arg,
        enable_when<(sizeof...(A) > sizeof...(Arg))> = enabler>
inline auto unpackAndApplyImpl(std::tuple<A ...> &&tuple, Arg&& ...arg) -> decltype(Func()(A() ...)) {
    return unpackAndApplyImpl<Func>(std::move(tuple),
                              std::forward<Arg>(arg)..., std::move(std::get<sizeof...(Arg)>(tuple)));
}


template <typename Func, typename ... A>
inline auto unpackAndApply(std::tuple<A ...> &&tuple) -> decltype(Func()(A() ...)) {
    return unpackAndApplyImpl<Func>(std::move(tuple));
};

template <typename Func, typename A>
inline auto unpackAndApply(A &&arg) -> decltype(Func()(std::forward<A>(arg))) {
    return Func()(std::forward<A>(arg));
};

template <typename Func>
inline auto unpackAndApply(unit &&arg) -> decltype(Func()()) {
    return Func()();
}

/**
 * construct object with tuple argument.
 */
template <typename T, typename ... Arg,
        enable_when<!is_specialization_of<T, std::unique_ptr>::value> = enabler>
inline T construct(Arg && ...arg) {
    return T(std::forward<Arg>(arg)...);
};

template <typename T, typename ... Arg,
        enable_when<is_specialization_of<T, std::unique_ptr>::value> = enabler>
inline T construct(Arg && ...arg) {
    using P = param_type_of_t<T>;
    return T(new P(std::forward<Arg>(arg)...));
};

template <typename T, typename ... A, typename ... Arg,
        enable_when<(sizeof...(A) == sizeof...(Arg))> = enabler>
inline T unpackAndConstructImpl(std::tuple<A ...> &&tuple, Arg&& ...arg) {
    return construct<T>(std::forward<Arg>(arg)...);
}

template <typename T, typename ... A, typename ... Arg,
        enable_when<(sizeof...(A) > sizeof...(Arg))> = enabler>
inline T unpackAndConstructImpl(std::tuple<A ...> &&tuple, Arg&& ...arg) {
    return unpackAndConstructImpl<T>(
            std::move(tuple), std::forward<Arg>(arg)..., std::move(std::get<sizeof...(Arg)>(tuple)));
}


template <typename T, typename ... A>
inline T unpackAndConstruct(std::tuple<A ...> &&tuple) {
    return unpackAndConstructImpl<T>(std::move(tuple));
};

template <typename T, typename A>
inline T unpackAndConstruct(A &&arg) {
    return construct<T>(std::forward<A>(arg));
};

template <typename T>
inline T unpackAndConstruct(unit &&arg) {
    return construct<T>();
}


} // namespace misc
} // namespace aquarius

#endif //AQUARIUS_CXX_INTERNAL_TUPLES_HPP
