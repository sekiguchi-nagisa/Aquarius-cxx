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

#ifndef AQUARIUS_CXX_INTERNAL_EXPRESSION_HPP
#define AQUARIUS_CXX_INTERNAL_EXPRESSION_HPP

#include <stdexcept>
#include <vector>
#include <string>
#include <cstdint>
#include <functional>

#include "state.hpp"
#include "misc.hpp"
#include "tuples.hpp"

namespace aquarius {

namespace misc {

// ascii map
constexpr std::uint64_t setBit(std::uint64_t bitmap, char ch) {
    return ch >= 0 && ch < 64 ? bitmap | (1L << ch) : throw std::logic_error("");
}

struct AsciiMap {
    std::uint64_t map[2];

    constexpr AsciiMap() : map{0, 0} { }

    constexpr AsciiMap(std::uint64_t upper, std::uint64_t lower) : map{upper, lower} { }

    constexpr AsciiMap operator+(AsciiMap asciiMap) const {
        return AsciiMap(this->map[0] | asciiMap.map[0], this->map[1] | asciiMap.map[1]);
    }

    constexpr AsciiMap operator+(char ch) const {
        return ch >= 0 && ch < 64 ? AsciiMap(setBit(this->map[0], ch), this->map[1]) :
               ch >= 64 ? AsciiMap(this->map[0], setBit(this->map[1], ch - 64)) :
               throw std::logic_error("must be ascii character");
    }

    bool contains(char ch) const {
        if(ch < 0) {
            return false;
        }
        if(ch < 64) {
            return this->map[0] & (1L << ch);
        }
        return this->map[1] & (1L << (ch - 64));
    }
};

constexpr AsciiMap update(AsciiMap asciiMap) {
    return asciiMap;
}

template <typename ... T>
constexpr AsciiMap update(AsciiMap asciiMap, AsciiMap other, T ... rest) {
    return update(asciiMap + other, rest...);
}

template <typename ... T>
constexpr AsciiMap update(AsciiMap asciiMap, char ch, T ... rest) {
    return update(asciiMap + ch, rest...);
}

template <typename ... T>
constexpr AsciiMap makeAsciiMap(T ... rest) {
    return update(AsciiMap(), rest...);
}

constexpr AsciiMap makeFromRange(AsciiMap asciiMap, char start, char stop) {
    return start < stop ? makeFromRange(asciiMap + start, start + 1, stop) : asciiMap + start;
}

constexpr AsciiMap makeFromRange(char start, char stop) {
    return start < stop ? makeFromRange(AsciiMap(), start, stop)
                        : throw std::logic_error("start is less than stop");
}

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
                            L, decltype(catAsTuple(L(), R()))
                    >::type
            >::type
    >::type;

} // namespace misc

namespace expression {

struct Expression { };

template <typename T>
struct is_expr : std::is_base_of<Expression, T> { };

struct Empty : Expression {
    using retType = unit;

    template <typename Iterator>
    unit operator()(ParserState<Iterator> &state) const {
        return unit();
    }
};

struct Any : Expression {
    using retType = unit;

    constexpr Any() { }

    template <typename Iterator>
    unit operator()(ParserState<Iterator> &state) const {
        if(state.cursor() == state.end()) {
            state.reportFailure();
        } else {
            ++state.cursor();
        }
        return unit();
    }
};

struct StringLiteral : Expression {
    using retType = unit;

    std::size_t size;
    const char *text;

    constexpr explicit StringLiteral(const char *text, std::size_t size) :
            text(text), size(size) { }

    template <typename Iterator>
    unit operator()(ParserState<Iterator> &state) const {
        if(state.end() - state.cursor() < this->size) {
            state.reportFailure();
        } else {
            auto old = state.cursor();
            for(unsigned int i = 0; i < this->size; i++) {
                if(this->text[i] != *state.cursor()) {
                    state.reportFailure();
                    state.cursor() = old;
                    break;
                }
                ++state.cursor();
            }
        }
        return unit();
    }
};


struct Char : Expression {
    using retType = unit;

    char ch;

    constexpr Char(char ch) : ch(ch) { }

    template <typename Iterator>
    unit operator()(ParserState<Iterator> &state) const {
        if(state.cursor() != state.end() && *state.cursor() == this->ch) {
            ++state.cursor();
        } else {
            state.reportFailure();
        }
        return unit();
    }
};

struct CharClass : Expression {
    using retType = unit;

    misc::AsciiMap asciiMap;

    constexpr explicit CharClass(misc::AsciiMap asciiMap) : asciiMap(asciiMap) { }

    template <typename Iterator>
    unit operator()(ParserState<Iterator> &state) const {
        if(state.cursor() == state.end()) {
            state.reportFailure();
        } else if(!this->asciiMap.contains(*state.cursor())) {
            state.reportFailure();
        } else {
            ++state.cursor();
        }
        return unit();
    }
};

template <typename T, typename D, size_t Low = 0, size_t High = static_cast<size_t>(-1)>
struct Repeat : Expression {
    static_assert(is_expr<T>::value, "must be Expression");
    static_assert(is_expr<D>::value, "must be Expression");
    static_assert(misc::is_unit<typename D::retType>::value, "must be unit expression");
    static_assert(Low < High, "invalid interval");

    using exprType = typename T::retType;
    using retType = misc::unaryRetTypeHelper<exprType, std::vector<exprType>>;

    T expr;
    D delim;

    constexpr Repeat(T expr, D delim) : expr(expr), delim(delim) { }

    template <typename Iterator, typename P = exprType,
            misc::enable_when<misc::is_unit<P>::value> = misc::enabler>
    unit operator()(ParserState<Iterator> &state) const {
        size_t index = 0;
        for(; index < High; index++) {
            // match delimiter
            if(!std::is_same<D, Empty>::value && index > 0) {
                this->delim(state);
                if(!state.result()) {
                    break;
                }
            }

            // match expression
            this->expr(state);
            if(!state.result()) {
                break;
            }
        }

        if(index >= Low) {
            state.setResult(true);
        }
        return unit();
    }

    template <typename Iterator, typename P = exprType,
            misc::enable_when<!misc::is_unit<P>::value> = misc::enabler>
    std::vector<exprType> operator()(ParserState<Iterator> &state) const {
        std::vector<exprType> value;

        size_t index = 0;
        for(; index < High; index++) {
            // match delimiter
            if(!std::is_same<D, Empty>::value && index > 0) {
                this->delim(state);
                if(!state.result()) {
                    break;
                }
            }

            // match expression
            auto v = this->expr(state);
            if(!state.result()) {
                break;
            }

            value.push_back(std::move(v));
        }

        if(index >= Low) {
            state.setResult(true);
        }
        return std::move(value);
    }
};

template <typename T>
struct Option : Expression {
    static_assert(is_expr<T>::value, "must be Expression");

    using exprType = typename T::retType;
    using retType = misc::unaryRetTypeHelper<exprType, Optional<exprType>>;

    T expr;

    constexpr explicit Option(T expr) : expr(expr) { }

    template <typename Iterator, typename P = exprType,
            misc::enable_when<misc::is_unit<P>::value> = misc::enabler>
    unit operator()(ParserState<Iterator> &state) const {
        this->expr(state);
        if(!state.result()) {
            state.setResult(true);
        }
        return unit();
    }

    template <typename Iterator, typename P = exprType,
            misc::enable_when<!misc::is_unit<P>::value> = misc::enabler>
    Optional<exprType> operator()(ParserState<Iterator> &state) const {
        Optional<exprType> value;
        auto v = this->expr(state);
        if(state.result()) {
            value = Optional<exprType>(std::move(v));
        } else {
            state.setResult(true);
        }
        return std::move(value);
    }
};

template <typename T>
struct NotPredicate : Expression {
    static_assert(is_expr<T>::value, "must be Expression");

    using exprType = typename T::retType;

    static_assert(misc::is_unit<exprType>::value, "must be unit type");

    using retType = unit;

    T expr;

    constexpr explicit NotPredicate(T expr) : expr(expr) { }

    template <typename Iterator>
    unit operator()(ParserState<Iterator> &state) const {
        auto old = state.cursor();
        this->expr(state);
        if(state.result()) {
            state.reportFailure();
            state.cursor() = old;
        } else {
            state.setResult(true);
        }
        return unit();
    }
};

template <typename T>
struct Capture : Expression {
    static_assert(is_expr<T>::value, "must be Expression");

    using exprType = typename T::retType;
    using retType = std::string;

    static_assert(misc::is_unit<exprType>::value, "must be unit type");

    T expr;

    constexpr explicit Capture(T expr) : expr(expr) { }

    template <typename Iterator>
    std::string operator()(ParserState<Iterator> &state) const {
        std::string str;
        auto old = state.cursor();
        this->expr(state);
        if(state.result()) {
            str = std::string(old, state.cursor());
        }
        return str;
    }
};

struct CaptureHolder {
    constexpr CaptureHolder() { }

    template <typename T>
    constexpr Capture<T> operator[](T expr) const {
        return Capture<T>(expr);
    }
};

template <typename L, typename R>
struct Sequence : Expression {
    static_assert(is_expr<L>::value && is_expr<R>::value, "must be Expression");

    using leftType = typename L::retType;
    using rightType = typename R::retType;

    using retType = misc::seqRetTypeHelper<leftType, rightType>;

    L left;
    R right;

    constexpr Sequence(L left, R right) : left(left), right(right) { }

    template <typename Iterator, typename LT = leftType, typename RT = rightType,
            misc::enable_when<misc::is_unit<LT>::value && misc::is_unit<RT>::value> = misc::enabler>
    unit operator()(ParserState<Iterator> &state) const {
        auto old = state.cursor();
        this->left(state);
        if(state.result()) {
            this->right(state);
            if(!state.result()) {
                state.cursor() = old;
            }
        }
        return unit();
    }

    // return left value
    template <typename Iterator, typename LT = leftType, typename RT = rightType,
            misc::enable_when<misc::is_unit<RT>::value && !misc::is_unit<LT>::value> = misc::enabler>
    leftType operator()(ParserState<Iterator> &state) const {
        auto old = state.cursor();
        auto v = this->left(state);
        if(state.result()) {
            this->right(state);
            if(!state.result()) {
                state.cursor() = old;
            }
        }
        return std::move(v);
    }

    // return right value
    template <typename Iterator, typename LT = leftType, typename RT = rightType,
            misc::enable_when<misc::is_unit<LT>::value && !misc::is_unit<RT>::value> = misc::enabler>
    rightType operator()(ParserState<Iterator> &state) const {
        auto old = state.cursor();
        rightType v;
        this->left(state);
        if(state.result()) {
            v = this->right(state);
            if(!state.result()) {
                state.cursor() = old;
            }
        }
        return std::move(v);
    }

    template <typename Iterator, typename LT = leftType, typename RT = rightType,
            misc::enable_when<!misc::is_unit<LT>::value && !misc::is_unit<RT>::value> = misc::enabler>
    retType operator()(ParserState<Iterator> &state) const {
        leftType v1;
        rightType v2;
        auto old = state.cursor();
        v1 = this->left(state);
        if(state.result()) {
            v2 = this->right(state);
            if(!state.result()) {
                state.cursor() = old;
            }
        }
        return misc::catAsTuple(std::move(v1), std::move(v2));
    }
};

template <typename L, typename R>
struct Choice : Expression {
    static_assert(is_expr<L>::value && is_expr<R>::value, "must be Expression");

    using leftType = typename L::retType;
    using rightType = typename R::retType;

    static_assert(std::is_assignable<leftType, rightType >::value, "must be assignable");

    using retType = leftType;

    L left;
    R right;

    constexpr Choice(L left, R right) : left(left), right(right) { }

    template <typename Iterator>
    retType operator()(ParserState<Iterator> &state) const {
        retType v = this->left(state);
        if(!state.result()) {
            state.setResult(true);
            v = this->right(state);
        }
        return std::move(v);
    }
};

template <typename T>
struct NonTerminal : Expression {
    using retType = misc::param_type_of_t<T>;

    constexpr NonTerminal() {}

    template <typename Iterator>
    retType operator()(ParserState<Iterator> &state) const {
        constexpr auto p = T::pattern();
        return p(state);
    }
};


// for mapper

struct Mapper {};

template <typename T>
struct is_mapper : std::is_base_of<Mapper, T> { };

template <typename T, typename M>
struct MapperAdapter : Expression {
    static_assert(is_expr<T>::value, "must be Expression");
    static_assert(is_mapper<M>::value, "must be Mapper");

    T expr;
    M mapper;

    using retType = typename M::retType;

    constexpr MapperAdapter(T expr, M mapper) : expr(expr), mapper(mapper) { }

    template <typename Iterator>
    retType operator()(ParserState<Iterator> &state) const {
        auto v = this->expr(state);
        retType r;
        if(state.result()) {
            r = this->mapper(state, std::move(v));
        }
        return std::move(r);
    }
};


} // namespace expression
} // namespace aquarius

#endif //AQUARIUS_CXX_INTERNAL_EXPRESSION_HPP
