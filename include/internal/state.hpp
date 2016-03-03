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

namespace aquarius {

template <typename RandomAccessIterator>
class ParserState {
private:
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

public:
    ParserState(RandomAccessIterator begin, RandomAccessIterator end) :
            begin_(begin), end_(end), cursor_(begin), result_(false) { }

    const RandomAccessIterator begin() const {
        return this->begin_;
    }

    const RandomAccessIterator end() const {
        return this->end_;
    }

    RandomAccessIterator &cursor() {
        return this->cursor_;
    }

    void reportFailure() {
        this->result_ = false;
    }

    operator bool() const noexcept {
        return this->result_;
    }
};

template <typename RandomAccessIterator>
inline ParserState<RandomAccessIterator> createState(RandomAccessIterator begin, RandomAccessIterator end) {
    return ParserState<RandomAccessIterator>(begin, end);
}

} // namespace aquarius

#endif //AQUARIUS_CXX_INTERNAL_STATE_HPP
