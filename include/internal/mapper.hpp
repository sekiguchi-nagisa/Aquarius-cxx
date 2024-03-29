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

#ifndef AQUARIUS_CXX_INTERNAL_MAPPER_HPP
#define AQUARIUS_CXX_INTERNAL_MAPPER_HPP

#include "expression.hpp"

namespace aquarius {
namespace mapper {

template <typename Functor>
struct CommonMapper : expression::Mapper {
    using retType = misc::ret_type_of_func_t<Functor>;
    static_assert(!std::is_void<retType>::value, "return type of Functor must not be void");

    template <typename Iterator, typename Value>
    auto operator()(ParserState<Iterator> &, Value &&v) const {
        return misc::unpackAndApply<Functor>(std::forward<Value>(v));
    }

    template <typename Iterator>
    auto operator()(ParserState<Iterator> &) const {
        return misc::unpackAndApply<Functor>();
    }
};

template <typename T>
struct Constructor : expression::Mapper {
    using retType = misc::type_of_constructor_t<T>;
    static_assert(!std::is_void<retType>::value, "must not be void");

    template <typename Iterator, typename Value>
    auto operator()(ParserState<Iterator> &, Value &&v) const {
        return misc::unpackAndConstruct<T>(std::forward<Value>(v));
    }

    template <typename Iterator>
    auto operator()(ParserState<Iterator> &) const {
        return misc::unpackAndConstruct<T>();
    }
};

template <typename T>
struct Supplier : expression::Mapper {
    using retType = T;
    static_assert(!std::is_void<retType>::value, "must not be void");

    T constant;

    constexpr explicit Supplier(T constant) : constant(constant) { }

    template <typename Iterator>
    auto operator()(ParserState<Iterator> &) const {
        return this->constant;
    }
};

template <typename T>
struct NullSupplier : expression::Mapper {
    using retType = std::unique_ptr<T>;

    template <typename Iterator>
    auto operator()(ParserState<Iterator> &) const {
        return std::unique_ptr<T>();
    }
};

template <typename T>
struct Convertible {
    template <typename U>
    bool operator()(const U &) const {
        return std::is_base_of<T, U>::value;
    }
};

template <typename T, typename C = Convertible<T>>
struct Cast : expression::Mapper {
    using retType = std::unique_ptr<T>;

    template <typename Iterator, typename U>
    auto operator()(ParserState<Iterator> &state, std::unique_ptr<U> &&value) const {
        static_assert(std::is_base_of<T, U>::value || std::is_base_of<U, T>::value, "must be base type of derived type");
        if(!C()(*value.get())) {
            state.setResult(false);
            return std::unique_ptr<T>();
        }
        return std::unique_ptr<T>(static_cast<T *>(value.release()));
    }
};

template <typename Functor, typename T>
struct JoinerBase : expression::Mapper {
    static_assert(expression::is_expr<T>::value, "must be Expression");

    using retType =
        typename std::remove_reference<
                misc::first_param_type_of_func_t<Functor>
        >::type;

    static_assert(!std::is_void<retType>::value, "first parameter type of Functor must not be void type");

    static_assert(std::is_void<misc::ret_type_of_func_t<Functor>>::value, "return type of Functor must be void");

    T expr;

    constexpr explicit JoinerBase(T expr) : expr(expr) { }
};


template <typename Functor, typename T>
struct Joiner : JoinerBase<Functor, T> {
    constexpr explicit Joiner(T expr) : JoinerBase<Functor, T>(expr) { }

    template <typename Iterator, typename Value>
    auto operator()(ParserState<Iterator> &state, Value &&v) const {
        auto r = this->expr(state);
        if(state.result()) {
            misc::unpackAndAppend<Functor>(v, std::forward<Value>(r));
        }
        return std::forward<Value>(v);
    }
};

template <typename Functor, typename T, typename D, size_t Low = 0, size_t High = static_cast<size_t>(-1)>
struct EachJoiner : JoinerBase<Functor, T> {
    static_assert(expression::is_expr<D>::value &&
                          std::is_void<typename D::retType>::value, "must be void type expression");

    D delim;

    constexpr EachJoiner(T expr, D delim) : JoinerBase<Functor, T>(expr), delim(delim) { }

    static bool isGreaterThan(size_t index, size_t limit) {
        return index >= limit;
    }

    template <typename Iterator, typename Value>
    auto operator()(ParserState<Iterator> &state, Value &&v) const {
        size_t index = 0;
        for(; index < High; index++) {
            // match delimiter
            if(!std::is_same<D, expression::Empty>::value && index > 0) {
                this->delim(state);
                if(!state.result()) {
                    break;
                }
            }

            // match expression
            auto r = this->expr(state);
            if(!state.result()) {
                break;
            }

            misc::unpackAndAppend<Functor>(v, std::move(r));
        }

        if(isGreaterThan(index, Low)) {
            state.setResult(true);
        }
        return std::forward<Value>(v);
    }
};


} // namespace mapper
} // namespace aquarius

#endif //AQUARIUS_CXX_INTERNAL_MAPPER_HPP
