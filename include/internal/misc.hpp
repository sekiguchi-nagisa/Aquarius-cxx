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
#include <iterator>

namespace aquarius {
namespace misc {

/**
 * type helper
 */
template <bool cond>
using enable_when = std::enable_if_t<cond, std::nullptr_t>;

/**
 * check whether a type is specialization of a specified template.
 */
template <typename C, template <typename ...> class T>
struct is_specialization_of : std::false_type { };

template <template <typename ...> class T, typename ... P>
struct is_specialization_of<T<P...>, T> : std::true_type { };

/**
 * get first type of type parameter pack.
 */
template <typename ... Arg>
struct first_of_param_pack { };

template <typename First, typename ... Arg>
struct first_of_param_pack<First, Arg ...> {
    using type = First;
};

template <typename ... T>
using first_of_param_pack_t = typename first_of_param_pack<T...>::type;

/**
 * get first type parameter of specialized template.
 */
template <typename T>
struct param_type_of { };

template <template <typename ...> class T, typename ... P>
struct param_type_of<T<P ...>> {
    using paramType = first_of_param_pack_t<P ...>;
};

template <typename T>
using param_type_of_t = typename param_type_of<T>::paramType;


/**
 * for function object traits.
 */
template <typename T>
struct func_type_traits : func_type_traits<decltype(&T::operator())> { };

template <typename Holder, typename Ret>
struct func_type_traits<Ret(Holder::*)() const> {
    using ret_type = Ret;
    using first_param_type = void;
};

template <typename Holder, typename Ret>
struct func_type_traits<Ret(Holder::*)()> {
    using ret_type = Ret;
    using first_param_type = void;
};

template <typename Holder, typename Ret, typename ... Arg>
struct func_type_traits<Ret(Holder::*)(Arg ...) const> {
    using ret_type = Ret;
    using first_param_type = first_of_param_pack_t<Arg ...>;
};

template <typename Holder, typename Ret, typename ... Arg>
struct func_type_traits<Ret(Holder::*)(Arg ...)> {
    using ret_type = Ret;
    using first_param_type = first_of_param_pack_t<Arg ...>;
};

/**
 * get return type of function object.
 */
template <typename T>
using ret_type_of_func_t = typename func_type_traits<T>::ret_type;

/**
 * get first parameter type of function object.
 */
template <typename T>
using first_param_type_of_func_t = typename func_type_traits<T>::first_param_type;

template <typename T>
using is_random_access_iter =
typename std::is_same<typename std::iterator_traits<T>::iterator_category, std::random_access_iterator_tag>;


template <typename T>
inline T constexpr_error(const char *) {
    abort();
}

template <typename T>
class NonCopyable {
protected:
    NonCopyable() = default;
    ~NonCopyable() = default;

public:
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

    explicit Optional(T &&t) : value_(new T) {
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
