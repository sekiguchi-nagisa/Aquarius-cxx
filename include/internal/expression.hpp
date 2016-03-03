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

#include "state.hpp"
#include "misc.hpp"

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

} // namespace misc

namespace expression {

struct Expression { };

struct Any : Expression {
    using retType = unit;

    constexpr Any() { }

    template <typename Iterator>
    bool operator()(ParserState<Iterator> &state) const {
        if(state.cursor() == state.end()) {  //FIXME: error report
            return false;   //FIXME: UTF-8
        }
        ++state.cursor();
        return true;
    }
};

struct StringLiteral : Expression {
    using retType = unit;

    std::size_t size;
    const char *text;

    constexpr explicit StringLiteral(const char *text, std::size_t size) :
            text(text), size(size) { }

    template <typename Iterator>
    bool operator()(ParserState<Iterator> &state) const {
        if(state.end() - state.cursor() < this->size) {
            return false;
        }

        auto old = state.cursor();
        for(unsigned int i = 0; i < this->size; i++) {
            if(this->text[i] != *state.cursor()) {
                state.cursor() = old;
                return false;
            }
            ++state.cursor();
        }
        return true;
    }
};


struct Char : Expression {
    using retType = unit;

    char ch;

    constexpr Char(char ch) : ch(ch) { }

    template <typename Iterator>
    bool operator()(ParserState<Iterator> &state) const {
        if(state.cursor() != state.end() && *state.cursor() == this->ch) {
            ++state.cursor();
            return true;
        }
        return false;
    }
};

struct CharClass : Expression {
    using retType = unit;

    misc::AsciiMap asciiMap;

    constexpr explicit CharClass(misc::AsciiMap asciiMap) : asciiMap(asciiMap) { }

    template <typename Iterator>
    bool operator()(ParserState<Iterator> &state) const {
        if(state.cursor() == state.end()) {
            return false;
        }
        if(!this->asciiMap.contains(*state.cursor())) {
            return false;
        }
        ++state.cursor();
        return true;
    }
};

template <typename T>
struct ZeroMore : Expression {
    static_assert(std::is_base_of<Expression, T>::value, "must be Expression");

    using exprType = typename T::retType;
    using retType = misc::unaryRetTypeHelper<exprType, std::vector<exprType>>;

    T expr;

    constexpr explicit ZeroMore(T expr) : expr(expr) { }

    template <typename Iterator>
    bool operator()(ParserState<Iterator> &state) const {
        while(this->expr(state));
        return true;
    }

    template <typename Iterator, typename P = retType>
    bool operator()(ParserState<Iterator> &state,
                    misc::enable_if_t<!misc::is_unit<P>::value, std::vector<exprType>> &value) const {
        exprType v;
        while(this->expr(state, v)) {
            value.push_back(std::move(v));
        }
        return true;
    }
};

template <typename T>
struct OneMore : Expression {
    static_assert(std::is_base_of<Expression, T>::value, "must be Expression");

    using exprType = typename T::retType;
    using retType = misc::unaryRetTypeHelper<exprType, std::vector<exprType>>;

    T expr;

    constexpr explicit OneMore(T expr) : expr(expr) { }

    template <typename Iterator>
    bool operator()(ParserState<Iterator> &state) const {
        if(!this->expr(state)) {
            return false;
        }
        while(this->expr(state));
        return true;
    }

    template <typename Iterator, typename P = retType>
    bool operator()(ParserState<Iterator> &state,
                    misc::enable_if_t<!misc::is_unit<P>::value, std::vector<exprType>> &value) const {
        exprType v;
        if(!this->expr(state, v)) {
            return false;
        }
        do {
            value.push_back(std::move(v));
        } while(this->expr(state, v));
        return true;
    }
};

template <typename T>
struct AndPredicate : Expression {
    static_assert(std::is_base_of<Expression, T>::value, "must be Expression");

    using exprType = typename T::retType;

    static_assert(misc::is_unit<exprType>::value, "must be unit type");

    using retType = unit;

    T expr;

    constexpr explicit AndPredicate(T expr) : expr(expr) { }

    template <typename Iterator>
    bool operator()(ParserState<Iterator> &state) const {
        auto old = state.cursor();
        if(!this->expr(state)) {
            return false;
        }
        state.cursor() = old;
        return true;
    }
};

template <typename T>
struct NotPredicate : Expression {
    static_assert(std::is_base_of<Expression, T>::value, "must be Expression");

    using exprType = typename T::retType;

    static_assert(misc::is_unit<exprType>::value, "must be unit type");

    using retType = unit;

    T expr;

    constexpr explicit NotPredicate(T expr) : expr(expr) { }

    template <typename Iterator>
    bool operator()(ParserState<Iterator> &state) const {
        auto old = state.cursor();
        if(this->expr(state)) {
            state.cursor() = old;
            return false;
        }
        return true;
    }
};

template <typename T>
struct Capture : Expression {
    static_assert(std::is_base_of<Expression, T>::value, "must be Expression");

    using exprType = typename T::retType;
    using retType = std::string;

    static_assert(misc::is_unit<exprType>::value, "must be unit type");

    T expr;

    constexpr explicit Capture(T expr) : expr(expr) { }

    template <typename Iterator>
    bool operator()(ParserState<Iterator> &state, std::string &value) const {
        auto old = state.cursor();
        if(!this->expr(state)) {
            return false;
        }
        value = std::string(old, state.cursor());
        return true;
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
    static_assert(std::is_base_of<Expression, L>::value &&
                  std::is_base_of<Expression, R>::value, "must be Expression");

    using leftType = typename L::retType;
    using rightType = typename R::retType;

    using retType = misc::seqRetTypeHelper<leftType, rightType>;

    L left;
    R right;

    constexpr Sequence(L left, R right) : left(left), right(right) { }

    template <typename Iterator>
    bool operator()(ParserState<Iterator> &state) const {
        auto old = state.cursor();
        if(!this->left(state)) {
            return false;
        }
        if(!this->right(state)) {
            state.cursor() = old;
            return false;
        }
        return true;
    }

    // return left value
    template <typename Iterator, typename LT = leftType, typename RT = rightType,
            misc::enable_when<misc::is_unit<RT>::value && !misc::is_unit<LT>::value> = misc::enabler>
    bool operator()(ParserState<Iterator> &state,
                    misc::enable_if_t<!misc::is_unit<LT>::value, retType> &value) const {
        auto old = state.cursor();
        if(!this->left(state, value)) {
            return false;
        }
        if(!this->right(state)) {
            state.cursor() = old;
            return false;
        }
        return true;
    }

    // return right value
    template <typename Iterator, typename LT = leftType, typename RT = rightType,
            misc::enable_when<misc::is_unit<LT>::value && !misc::is_unit<RT>::value> = misc::enabler>
    bool operator()(ParserState<Iterator> &state,
                    misc::enable_if_t<!misc::is_unit<RT>::value, retType> &value) const {
        auto old = state.cursor();
        if(!this->left(state)) {
            return false;
        }
        if(!this->right(state, value)) {
            state.cursor() = old;
            return false;
        }
        return true;
    }

    template <typename Iterator, typename LT = leftType, typename RT = rightType,
            misc::enable_when<!misc::is_unit<LT>::value && !misc::is_unit<RT>::value> = misc::enabler>
    bool operator()(ParserState<Iterator> &state,
                    std::pair<leftType, rightType> &value) const {
        auto old = state.cursor();
        leftType v1;
        rightType v2;

        if(!this->left(state, v1)) {
            return false;
        }
        if(!this->right(state, v2)) {
            state.cursor() = old;
            return false;
        }
        value = std::make_pair(std::move(v1), std::move(v2));
        return true;
    }
};

template <typename T>
struct NonTerminal : Expression {
    using retType = typename T::retType;

    constexpr NonTerminal() {}

    template <typename Iterator>
    bool operator()(ParserState<Iterator> &state) const {
        return T::pattern()(state);
    }

    template <typename Iterator, typename P = retType>
    bool operator()(ParserState<Iterator> &state,
                    misc::enable_if_t<!misc::is_unit<P>::value, P> &value) const {
        return T::pattern()(state, value);
    }
};

} // namespace expression
} // namespace aquarius

#endif //AQUARIUS_CXX_INTERNAL_EXPRESSION_HPP
