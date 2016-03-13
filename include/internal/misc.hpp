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

namespace aquarius {

struct unit {}; // for representing empty value

namespace misc {

template <typename T>
struct is_unit : std::is_same<T, unit> { };

/**
 * type helper
 */
extern void *enabler;

template <bool cond, typename T>
using enable_if_t = typename std::enable_if<cond, T>::type;

template <bool cond>
using enable_when = enable_if_t<cond, void> *&;

template <typename T>
struct param_type_of { };

template <template <typename> class T, typename P>
struct param_type_of<T<P>> {
    using paramType = P;
};

template <typename T>
using param_type_of_t = typename param_type_of<T>::paramType;

template <typename T>
struct ret_type_of_func { };

template <typename Holder, typename Ret, typename ... Arg>
struct ret_type_of_func<Ret(Holder::*)(Arg ...) const> {
    using type = Ret;
};

template <typename Holder, typename Ret, typename ... Arg>
struct ret_type_of_func<Ret(Holder::*)(Arg ...)> {
    using type = Ret;
};

template <typename T>
using ret_type_of_func_t = typename ret_type_of_func<decltype(&T::operator())>::type;


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
