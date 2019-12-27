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

#ifndef AQUARIUS_CXX_INTERNAL_STATE_HPP
#define AQUARIUS_CXX_INTERNAL_STATE_HPP

#include "misc.hpp"

namespace aquarius {

template <typename RandomAccessIterator>
class ParserState {
private:
    static_assert(misc::is_random_access_iter<RandomAccessIterator>::value, "require random access iterator");

    RandomAccessIterator begin_;
    RandomAccessIterator end_;

    /**
     * indicating current input position.
     */
    RandomAccessIterator cursor_;

    /**
     * indicating parser result (success or failure).
     */
    bool result_;

    /**
     * indicate longest matched failure
     */
    RandomAccessIterator failure_;

public:
    ParserState(RandomAccessIterator begin, RandomAccessIterator end) :
            begin_(begin), end_(end), cursor_(begin), result_(true), failure_(begin) { }

    RandomAccessIterator begin() const {
        return this->begin_;
    }

    RandomAccessIterator end() const {
        return this->end_;
    }

    RandomAccessIterator &cursor() {
        return this->cursor_;
    }

    size_t consumedSize() const {
        return std::distance(this->begin_, this->cursor_);
    }

    size_t remainedSize() const {
        return std::distance(this->cursor_, this->end_);
    }

    void reportFailure() {
        this->result_ = false;
        if(this->cursor_ > this->failure_) {
            this->failure_ = this->cursor_;
        }
    }

    size_t failurePos() const {
        return std::distance(this->cursor_, this->failure_);
    }

    void setResult(bool set) {
        this->result_ = set;
    }

    bool result() const {
        return this->result_;
    }
};

template <typename RandomAccessIterator>
inline ParserState<RandomAccessIterator> createState(RandomAccessIterator begin, RandomAccessIterator end) {
    return ParserState<RandomAccessIterator>(begin, end);
}

} // namespace aquarius

#endif //AQUARIUS_CXX_INTERNAL_STATE_HPP
