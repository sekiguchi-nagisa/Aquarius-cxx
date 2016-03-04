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

#ifndef AQUARIUS_CXX_INTERNAL_MISC_HPP
#define AQUARIUS_CXX_INTERNAL_MISC_HPP

#include <type_traits>
#include <tuple>

namespace aquarius {

struct unit {}; // for representing empty value

namespace misc {

template <typename T>
struct is_unit {
    static constexpr bool value = std::is_same<T, unit>::value;
};

/**
 * type helper
 */
extern void *enabler;

template <bool cond, typename T>
using enable_if_t = typename std::enable_if<cond, T>::type;

template <bool cond>
using enable_when = enable_if_t<cond, void> *&;

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

/**
 * for expression return type resolving
 */
template <typename T, typename V>
using unaryRetTypeHelper =
    typename std::conditional< is_unit<T>::value,
            unit, V
    >::type;

template <typename L, typename R>
using seqRetTypeHelper =
    typename std::conditional< is_unit<L>::value && is_unit<R>::value,
            unit, typename std::conditional< is_unit<L>::value,
                    R, typename std::conditional< is_unit<R>::value,
                            L, decltype(appendToTuple(L(), R()))
                    >::type
            >::type
    >::type;

/**
 * check whether value is constant or not
 */
template <typename T>
constexpr bool isConstant(T) {
    return true;
}

template <typename T>
class NonCopyable {
protected:
    NonCopyable() = default;
    ~NonCopyable() = default;

private:
    NonCopyable(const NonCopyable<T> &) = delete;
    NonCopyable<T> &operator=(const NonCopyable<T> &) = delete;
};

} // namespace misc

template <typename T>
class Optional : public misc::NonCopyable<Optional<T>> {
private:
    T *value_;

public:
    Optional() : value_(nullptr) { }

    Optional(T &&t) : value_(new T) {
        *this->value_ = std::move(t);
    }

    Optional(Optional &&o) : value_(o.value_) {
        o.value_ = nullptr;
    }

    ~Optional() {
        delete this->value_;
    }

    Optional &operator=(Optional &&o) {
        auto tmp(std::move(o));
        this->swap(tmp);
        return *this;
    }

    void swap(Optional &o) {
        std::swap(this->value_, o.value_);
    }

    explicit operator bool() const {
        return this->value_ != nullptr;
    }

    T &get() {
        return *this->value_;
    }
};

} // namespace aquarius

#endif //AQUARIUS_CXX_INTERNAL_MISC_HPP
