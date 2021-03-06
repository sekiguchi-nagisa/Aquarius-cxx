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
#include "internal/parser.hpp"
#include "internal/combinator.hpp"

// helper macro
#define aquarius_pattern_t constexpr auto

#define AQ_DEFINE_RULE(name, ...) \
template <typename T>             \
struct name ## __impl {           \
    static constexpr auto pattern();                                  \
}; using name = name ## __impl<__VA_ARGS__>;  \
template<typename T> constexpr auto name ## __impl<T>::pattern()

#define AQ_DECL_RULE(name, ...) \
template <typename T> struct name ## __impl; \
using name = name ## __impl<__VA_ARGS__>

#endif //AQUARIUS_CXX_AQUARIUS_HPP
