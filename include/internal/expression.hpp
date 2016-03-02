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
#include <type_traits>
#include <cstdint>

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
    typedef void retType;

    constexpr Any() { }

    template <typename RandomAccessIterator>
    bool operator()(RandomAccessIterator &begin, const RandomAccessIterator end) const {
        if(begin == end) {  //FIXME: error report
            return false;   //FIXME: UTF-8
        }
        ++begin;
        return true;
    }
};

struct StringLiteral : Expression {
    typedef void retType;

    std::size_t size;
    const char *text;

    constexpr explicit StringLiteral(const char *text, std::size_t size) :
            text(text), size(size) { }

    template <typename RandomAccessIterator>
    bool operator()(RandomAccessIterator &begin, const RandomAccessIterator end) const {
        if(end - begin < this->size) {
            return false;
        }

        RandomAccessIterator old = begin;
        for(unsigned int i = 0; i < this->size; i++) {
            if(this->text[i] != *begin) {
                begin = old;
                return false;
            }
            ++begin;
        }
        return true;
    }
};


struct Char : Expression {
    typedef void retType;

    char ch;

    constexpr Char(char ch) : ch(ch) { }

    template <typename RandomAccessIterator>
    bool operator()(RandomAccessIterator &begin, const RandomAccessIterator end) const {
        if(begin != end && *begin == this->ch) {
            ++begin;
            return true;
        }
        return false;
    }
};

struct CharClass : Expression {
    typedef void retType;

    misc::AsciiMap asciiMap;

    constexpr explicit CharClass(misc::AsciiMap asciiMap) : asciiMap(asciiMap) { }

    template <typename RandomAccessIterator>
    bool operator()(RandomAccessIterator &begin, const RandomAccessIterator end) const {
        if(begin == end) {
            return false;
        }
        if(!this->asciiMap.contains(*begin)) {
            return false;
        }
        ++begin;
        return true;
    }
};

template <typename T, typename V>
using unaryRetTypeHelper =
typename std::conditional< std::is_void<T>::value,
        void, V
>::type;

template <typename T>
struct ZeroMore : Expression {
    static_assert(std::is_base_of<Expression, T>::value, "must be Expression");

    typedef typename T::retType exprType;
    typedef unaryRetTypeHelper<exprType, std::vector<exprType>> retType;

    T expr;

    constexpr explicit ZeroMore(T expr) : expr(expr) { }

    template <typename RandomAccessIterator>
    bool operator()(RandomAccessIterator &begin, const RandomAccessIterator end) const {
        while(this->expr(begin, end));
        return true;
    }

    template <typename RandomAccessIterator, typename P = retType>
    bool operator()(RandomAccessIterator &begin, const RandomAccessIterator end,
                    misc::enable_if_t<!std::is_void<P>::value, std::vector<exprType>> &value) const {
        exprType v;
        while(this->expr(begin, end, v)) {
            value.push_back(std::move(v));
        }
        return true;
    }
};

template <typename T>
struct OneMore : Expression {
    static_assert(std::is_base_of<Expression, T>::value, "must be Expression");

    typedef typename T::retType exprType;

    typedef unaryRetTypeHelper<exprType, std::vector<exprType>> retType;

    T expr;

    constexpr explicit OneMore(T expr) : expr(expr) { }

    template <typename RandomAccessIterator>
    bool operator()(RandomAccessIterator &begin, const RandomAccessIterator end) const {
        if(!this->expr(begin, end)) {
            return false;
        }
        while(this->expr(begin, end));
        return true;
    }

    template <typename RandomAccessIterator, typename P = retType>
    bool operator()(RandomAccessIterator &begin, const RandomAccessIterator end,
                    misc::enable_if_t<!std::is_void<P>::value, std::vector<exprType>> &value) const {
        exprType v;
        if(!this->expr(begin, end, v)) {
            return false;
        }
        do {
            value.push_back(std::move(v));
        } while(this->expr(begin, end, v));
        return true;
    }
};

template <typename T>
struct AndPredicate : Expression {
    static_assert(std::is_base_of<Expression, T>::value, "must be Expression");

    typedef typename T::retType exprType;

    static_assert(std::is_void<exprType>::value, "must be void type");

    typedef void retType;

    T expr;

    constexpr explicit AndPredicate(T expr) : expr(expr) { }

    template <typename RandomAccessIterator>
    bool operator()(RandomAccessIterator &begin, const RandomAccessIterator end) const {
        RandomAccessIterator old = begin;
        if(!this->expr(begin, end)) {
            return false;
        }
        begin = old;
        return true;
    }
};

template <typename T>
struct NotPredicate : Expression {
    static_assert(std::is_base_of<Expression, T>::value, "must be Expression");

    typedef typename T::retType exprType;

    static_assert(std::is_void<exprType>::value, "must be void type");

    typedef void retType;

    T expr;

    constexpr explicit NotPredicate(T expr) : expr(expr) { }

    template <typename RandomAccessIterator>
    bool operator()(RandomAccessIterator &begin, const RandomAccessIterator end) const {
        RandomAccessIterator old = begin;
        if(this->expr(begin, end)) {
            begin = old;
            return false;
        }
        return true;
    }
};

template <typename T>
struct Capture : Expression {
    static_assert(std::is_base_of<Expression, T>::value, "must be Expression");

    typedef typename T::retType exprType;

    typedef std::string retType;

    static_assert(std::is_void<exprType>::value, "must be void type");

    T expr;

    constexpr explicit Capture(T expr) : expr(expr) { }

    template <typename RandomAccessIterator>
    bool operator()(RandomAccessIterator &begin, const RandomAccessIterator end, std::string &value) const {
        RandomAccessIterator old = begin;
        if(!this->expr(begin, end)) {
            return false;
        }
        value = std::string(old, begin);
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

    typedef typename L::retType leftType;
    typedef typename R::retType rightType;

    typedef typename std::conditional< std::is_void<leftType>::value && std::is_void<rightType>::value,
            void, typename std::conditional< std::is_void<leftType>::value,
                    rightType, typename std::conditional< std::is_void<rightType>::value,
                            leftType, std::pair<leftType, rightType>
            >::type
    >::type
    >::type retType;

    L left;
    R right;

    constexpr Sequence(L left, R right) : left(left), right(right) { }

    template <typename RandomAccessIterator>
    bool operator()(RandomAccessIterator &begin, const RandomAccessIterator end) const {
        RandomAccessIterator old = begin;
        if(!this->left(begin, end)) {
            return false;
        }
        if(!this->right(begin, end)) {
            begin = old;
            return false;
        }
        return true;
    }

    // return left value
    template <typename RandomAccessIterator, typename LT = leftType, typename RT = rightType,
            misc::enable_when<std::is_void<RT>::value && !std::is_void<LT>::value> = misc::enabler>
    bool operator()(RandomAccessIterator &begin, const RandomAccessIterator end,
                    misc::enable_if_t<!std::is_void<LT>::value, retType> &value) const {
        RandomAccessIterator old = begin;
        if(!this->left(begin, end, value)) {
            return false;
        }
        if(!this->right(begin, end)) {
            begin = old;
            return false;
        }
        return true;
    }

    // return right value
    template <typename RandomAccessIterator, typename LT = leftType, typename RT = rightType,
            misc::enable_when<std::is_void<LT>::value && !std::is_void<RT>::value> = misc::enabler>
    bool operator()(RandomAccessIterator &begin, const RandomAccessIterator end,
                    misc::enable_if_t<!std::is_void<RT>::value, retType> &value) const {
        RandomAccessIterator old = begin;
        if(!this->left(begin, end)) {
            return false;
        }
        if(!this->right(begin, end, value)) {
            begin = old;
            return false;
        }
        return true;
    }

    template <typename RandomAccessIterator, typename LT = leftType, typename RT = rightType,
            misc::enable_when<!std::is_void<LT>::value && !std::is_void<RT>::value> = misc::enabler>
    bool operator()(RandomAccessIterator &begin, const RandomAccessIterator end,
                    std::pair<leftType, rightType> &value) const {
        RandomAccessIterator old = begin;
        leftType v1;
        rightType v2;

        if(!this->left(begin, end, v1)) {
            return false;
        }
        if(!this->right(begin, end, v2)) {
            begin = old;
            return false;
        }
        value = std::make_pair(std::move(v1), std::move(v2));
        return true;
    }
};

template <typename T>
struct NonTerminal : Expression {
    typedef typename T::retType retType;

    constexpr NonTerminal() {}

    template <typename RandomAccessIterator>
    bool operator()(RandomAccessIterator &begin, const RandomAccessIterator end) const {
        return T::pattern()(begin, end);
    }

    template <typename RandomAccessIterator, typename P = retType>
    bool operator()(RandomAccessIterator &begin, const RandomAccessIterator end,
                    misc::enable_if_t<!std::is_void<P>::value, P> &value) const {
        return T::pattern()(begin, end, value);
    }
};

} // namespace expression
} // namespace aquarius

#endif //AQUARIUS_CXX_INTERNAL_EXPRESSION_HPP
