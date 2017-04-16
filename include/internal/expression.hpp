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
#include "unicode.hpp"

namespace aquarius {
namespace expression {

struct Expression { };

template <typename T>
struct is_expr : std::is_base_of<Expression, T> { };

template <typename T>
struct ExprBase : Expression {
    using retType = T;
};

struct Empty : ExprBase<void> {
    constexpr Empty() { }

    template <typename Iterator>
    void operator()(ParserState<Iterator> &) const { }
};

struct Any : ExprBase<void> {
    constexpr Any() { }

    template <typename Iterator>
    void operator()(ParserState<Iterator> &state) const {
        if(state.cursor() == state.end() || *state.cursor() < 0) {
            state.reportFailure();
        } else {
            ++state.cursor();
        }
    }
};

struct Utf8Any : ExprBase<void>, unicode_util::Utf8Util<true> {
    constexpr Utf8Any() { }

    template <typename Iterator>
    void operator()(ParserState<Iterator> &state) const {
        if(state.remainedSize() > 0) {
            unsigned int size = this->utf8ByteSize(*state.cursor());
            if(size > 0 && size < 5) {
                if(state.remainedSize() >= size) {
                    state.cursor() += size;
                    return;
                }
            }
        }
        state.reportFailure();
    }
};

struct StringLiteral : ExprBase<void> {
    std::size_t size;
    const char *text;

    constexpr explicit StringLiteral(const char *text, std::size_t size) :
            size(size), text(text) { }

    template <typename Iterator>
    void operator()(ParserState<Iterator> &state) const {
        if(state.cursor() + this->size > state.end()) {
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
    }
};


struct Char : ExprBase<void> {
    char ch;

    constexpr explicit Char(char ch) : ch(ch) { }

    template <typename Iterator>
    void operator()(ParserState<Iterator> &state) const {
        if(state.cursor() != state.end() && *state.cursor() == this->ch) {
            ++state.cursor();
        } else {
            state.reportFailure();
        }
    }
};

struct CharClass : ExprBase<void> {
    unicode_util::AsciiMap asciiMap;

    constexpr explicit CharClass(unicode_util::AsciiMap asciiMap) : asciiMap(asciiMap) { }

    template <typename Iterator>
    void operator()(ParserState<Iterator> &state) const {
        if(state.cursor() == state.end()) {
            state.reportFailure();
        } else if(!this->asciiMap.contains(*state.cursor())) {
            state.reportFailure();
        } else {
            ++state.cursor();
        }
    }
};

template <typename T>
struct UnaryExpr : Expression {
    static_assert(is_expr<T>::value, "must be Expression");

    T expr;

    constexpr explicit UnaryExpr(T expr) : expr(expr) { }
};

template <typename T, typename D, size_t Low, size_t High>
struct RepeatBaseCommon : UnaryExpr<T> {
    static_assert(is_expr<D>::value, "must be Expression");
    static_assert(std::is_void<typename D::retType>::value, "must be void type");
    static_assert(Low < High, "invalid interval");

    constexpr explicit RepeatBaseCommon(T expr) : UnaryExpr<T>(expr) {}
};

template <typename T, typename D, size_t Low, size_t High>
struct RepeatBase : RepeatBaseCommon<T, D, Low, High> {
    D delim;

    constexpr RepeatBase(T expr, D delim) : RepeatBaseCommon<T, D, Low, High>(expr), delim(delim) { }

    template <typename Iterator>
    bool matchDelim(ParserState<Iterator> &state, size_t index) const {
        if(index > 0) {
            this->delim(state);
            return state.result();
        }
        return true;
    }
};

template <typename T, size_t Low, size_t High>
struct RepeatBase<T, Empty, Low, High> : RepeatBaseCommon<T, Empty, Low, High> {
    constexpr RepeatBase(T expr, Empty) : RepeatBaseCommon<T, Empty, Low, High>(expr) { }

    template <typename Iterator>
    bool matchDelim(ParserState<Iterator> &, size_t) const {
        return true;
    }
};

template <typename T, typename D, size_t Low, size_t High>
struct RepeatVoid : RepeatBase<T, D, Low, High> {
    static_assert(std::is_void<typename T::retType>::value, "must be void type");

    using retType = void;

    constexpr RepeatVoid(T expr, D delim) : RepeatBase<T, D, Low, High>(expr, delim) { }

    template <typename Iterator>
    void operator()(ParserState<Iterator> &state) const {
        size_t index = 0;
        for(; index < High; index++) {
            // match delimiter
            if(!this->matchDelim(state, index)) {
                break;
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
    }
};


template <typename T, typename D, size_t Low, size_t High>
struct Repeat : RepeatBase<T, D, Low, High> {
    using exprType = typename T::retType;

    static_assert(!std::is_void<exprType>::value, "must not be void type");

    using retType = std::vector<exprType>;

    constexpr Repeat(T expr, D delim) : RepeatBase<T, D, Low, High>(expr, delim) { }

    template <typename Iterator>
    std::vector<exprType> operator()(ParserState<Iterator> &state) const {
        std::vector<exprType> value;

        size_t index = 0;
        for(; index < High; index++) {
            // match delimiter
            if(!this->matchDelim(state, index)) {
                break;
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

template <size_t Low, size_t High, typename T, typename D,
        misc::enable_when<is_expr<T>::value && std::is_void<typename T::retType>::value> = nullptr>
constexpr RepeatVoid<T, D, Low, High> repeatHelper(T expr, D delim) {
    return RepeatVoid<T, D, Low, High>(expr, delim);
}

template <size_t Low, size_t High, typename T, typename D,
        misc::enable_when<is_expr<T>::value && !std::is_void<typename T::retType>::value> = nullptr>
constexpr Repeat<T, D, Low, High> repeatHelper(T expr, D delim) {
    return Repeat<T, D, Low, High>(expr, delim);
}

template <typename T>
struct OptionVoid : UnaryExpr<T> {
    static_assert(std::is_void<typename T::retType>::value, "must be void type");

    using retType = void;

    constexpr explicit OptionVoid(T expr) : UnaryExpr<T>(expr) { }

    template <typename Iterator>
    void operator()(ParserState<Iterator> &state) const {
        this->expr(state);
        if(!state.result()) {
            state.setResult(true);
        }
    }
};

template <typename T>
struct Option : UnaryExpr<T> {
    using exprType = typename T::retType;
    static_assert(!std::is_void<exprType>::value, "must not be void type");

    using retType = Optional<exprType>;

    constexpr explicit Option(T expr) : UnaryExpr<T>(expr) { }

    template <typename Iterator>
    Optional<exprType> operator()(ParserState<Iterator> &state) const {
        Optional<exprType> value;
        auto v = this->expr(state);
        if(state.result()) {
            value = Optional<exprType>(std::move(v));
        } else {
            state.setResult(true);
        }
        return value;
    }
};

template <typename T,
        misc::enable_when<is_expr<T>::value && std::is_void<typename T::retType>::value> = nullptr>
constexpr OptionVoid<T> optionHelper(T expr) {
    return OptionVoid<T>(expr);
}

template <typename T,
        misc::enable_when<is_expr<T>::value && !std::is_void<typename T::retType>::value> = nullptr>
constexpr Option<T> optionHelper(T expr) {
    return Option<T>(expr);
}

template <typename T>
struct NotPredicate : UnaryExpr<T> {
    using exprType = typename T::retType;

    static_assert(std::is_void<exprType>::value, "must be void type");

    using retType = void;

    constexpr explicit NotPredicate(T expr) : UnaryExpr<T>(expr) { }

    template <typename Iterator>
    void operator()(ParserState<Iterator> &state) const {
        auto old = state.cursor();
        this->expr(state);
        if(state.result()) {
            state.reportFailure();
            state.cursor() = old;
        } else {
            state.setResult(true);
        }
    }
};

template <typename T>
struct Capture : ExprBase<std::string> {
    static_assert(is_expr<T>::value, "must be Expression");

    using exprType = typename T::retType;

    static_assert(std::is_void<exprType>::value, "must be void type");

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
struct BinaryExpr : Expression {
    static_assert(is_expr<L>::value && is_expr<R>::value, "must be Expression");

    L left;
    R right;

    constexpr BinaryExpr(L left, R right) : left(left), right(right) { }
};

template <typename L, typename R>
struct SequenceVoid : BinaryExpr<L, R> {
    static_assert(std::is_void<typename L::retType>::value
                  && std::is_void<typename R::retType>::value, "left and right expression must be void type");

    using retType = void;

    constexpr SequenceVoid(L left, R right) : BinaryExpr<L, R>(left, right) { }

    template <typename Iterator>
    void operator()(ParserState<Iterator> &state) const {
        auto old = state.cursor();
        this->left(state);
        if(state.result()) {
            this->right(state);
            if(!state.result()) {
                state.cursor() = old;
            }
        }
    }
};

template <typename L, typename R>
struct SequenceRightVoid : BinaryExpr<L, R> {
    static_assert(!std::is_void<typename L::retType>::value
                  && std::is_void<typename R::retType>::value, "right expression must be void type");

    using retType = typename L::retType;

    constexpr SequenceRightVoid(L left, R right) : BinaryExpr<L, R>(left, right) { }

    template <typename Iterator>
    retType operator()(ParserState<Iterator> &state) const {
        auto old = state.cursor();
        auto v = this->left(state);
        if(state.result()) {
            this->right(state);
            if(!state.result()) {
                state.cursor() = old;
            }
        }
        return v;
    }
};

template <typename L, typename R>
struct SequenceLeftVoid : BinaryExpr<L, R> {
    static_assert(std::is_void<typename L::retType>::value
                  && !std::is_void<typename R::retType>::value, "left expression must be void type");

    using retType = typename R::retType;

    constexpr SequenceLeftVoid(L left, R right) : BinaryExpr<L, R>(left, right) { }

    template <typename Iterator>
    retType operator()(ParserState<Iterator> &state) const {
        auto old = state.cursor();
        retType v;
        this->left(state);
        if(state.result()) {
            v = this->right(state);
            if(!state.result()) {
                state.cursor() = old;
            }
        }
        return v;
    }
};

template <typename L, typename R>
struct Sequence : BinaryExpr<L, R> {
    static_assert(!std::is_void<typename L::retType>::value
                  && !std::is_void<typename R::retType>::value, "left and right expression must not be void type");

    using leftType = typename L::retType;
    using rightType = typename R::retType;

    using retType = decltype(misc::catAsTuple(leftType(), rightType()));

    constexpr Sequence(L left, R right) : BinaryExpr<L, R>(left, right) { }

    template <typename Iterator>
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

template <typename L, typename R,
        misc::enable_when<is_expr<L>::value && is_expr<R>::value
                          && std::is_void<typename L::retType>::value
                          && std::is_void<typename R::retType>::value> = nullptr>
constexpr SequenceVoid<L, R> seqHelper(L left, R right) {
    return SequenceVoid<L, R>(left, right);
}

template <typename L, typename R,
        misc::enable_when<is_expr<L>::value && is_expr<R>::value
                          && !std::is_void<typename L::retType>::value
                          && std::is_void<typename R::retType>::value> = nullptr>
constexpr SequenceRightVoid<L, R> seqHelper(L left, R right) {
    return SequenceRightVoid<L, R>(left, right);
}

template <typename L, typename R,
        misc::enable_when<is_expr<L>::value && is_expr<R>::value
                          && std::is_void<typename L::retType>::value
                          && !std::is_void<typename R::retType>::value> = nullptr>
constexpr SequenceLeftVoid<L, R> seqHelper(L left, R right) {
    return SequenceLeftVoid<L, R>(left, right);
}

template <typename L, typename R,
        misc::enable_when<is_expr<L>::value && is_expr<R>::value
                          && !std::is_void<typename L::retType>::value
                          && !std::is_void<typename R::retType>::value> = nullptr>
constexpr Sequence<L, R> seqHelper(L left, R right) {
    return Sequence<L, R>(left, right);
}


template <typename L, typename R>
struct ChoiceVoid : BinaryExpr<L, R> {
    using leftType = typename L::retType;
    using rightType = typename R::retType;

    static_assert(std::is_void<leftType>::value && std::is_void<rightType>::value, "must be void type");

    using retType = void;

    constexpr ChoiceVoid(L left, R right) : BinaryExpr<L, R>(left, right) { }

    template <typename Iterator>
    void operator()(ParserState<Iterator> &state) const {
        this->left(state);
        if(!state.result()) {
            state.setResult(true);
            this->right(state);
        }
    }
};

template <typename L, typename R>
struct Choice : BinaryExpr<L, R> {
    using leftType = typename L::retType;
    using rightType = typename R::retType;

    static_assert(std::is_assignable<leftType, rightType >::value, "must be assignable");

    using retType = leftType;

    constexpr Choice(L left, R right) : BinaryExpr<L, R>(left, right) { }

    template <typename Iterator>
    retType operator()(ParserState<Iterator> &state) const {
        retType v = this->left(state);
        if(!state.result()) {
            state.setResult(true);
            v = this->right(state);
        }
        return v;
    }
};

template <typename L, typename R,
        misc::enable_when<is_expr<L>::value && is_expr<R>::value
                          && std::is_void<typename L::retType>::value
                          && std::is_void<typename R::retType>::value> = nullptr>
constexpr ChoiceVoid<L, R> choiceHelper(L left, R right) {
    return ChoiceVoid<L, R>(left, right);
}

template <typename L, typename R,
        misc::enable_when<is_expr<L>::value && is_expr<R>::value
                          && !std::is_void<typename L::retType>::value
                          && !std::is_void<typename R::retType>::value> = nullptr>
constexpr Choice<L, R> choiceHelper(L left, R right) {
    return Choice<L, R>(left, right);
}

template <typename T>
struct NonTerminal : Expression {
    using retType = misc::param_type_of_t<T>;

    constexpr NonTerminal() {}

    template <typename Iterator, typename P = retType,
            misc::enable_when<!std::is_void<P>::value> = nullptr>
    retType operator()(ParserState<Iterator> &state) const {
        constexpr auto p = T::pattern();
        return p(state);
    }

    template <typename Iterator, typename P = retType,
            misc::enable_when<std::is_void<P>::value> = nullptr>
    void operator()(ParserState<Iterator> &state) const {
        constexpr auto p = T::pattern();
        p(state);
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

    template <typename Iterator, typename P = typename T::retType,
            misc::enable_when<std::is_void<P>::value> = nullptr>
    retType operator()(ParserState<Iterator> &state) const {
        this->expr(state);
        auto r = retType();
        if(state.result()) {
            r = this->mapper(state);
        }
        return r;
    }

    template <typename Iterator, typename P = typename T::retType,
            misc::enable_when<!std::is_void<P>::value> = nullptr>
    retType operator()(ParserState<Iterator> &state) const {
        auto v = this->expr(state);
        auto r = retType();
        if(state.result()) {
            r = this->mapper(state, std::move(v));
        }
        return r;
    }
};


} // namespace expression
} // namespace aquarius

#endif //AQUARIUS_CXX_INTERNAL_EXPRESSION_HPP
