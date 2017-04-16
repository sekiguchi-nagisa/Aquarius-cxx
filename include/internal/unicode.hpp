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

#ifndef AQUARIUS_CXX_INTERNAL_ASCII_HPP
#define AQUARIUS_CXX_INTERNAL_ASCII_HPP

#include "misc.hpp"

namespace aquarius {
namespace unicode_util {

// ascii map
constexpr std::uint64_t setBit(std::uint64_t bitmap, char ch) {
    return ch >= 0 && ch < 64 ? bitmap | (1L << ch) : misc::constexpr_error<std::uint64_t>("");
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
               misc::constexpr_error<AsciiMap>("must be ascii character");
    }

    constexpr AsciiMap operator~() const {
        return AsciiMap(~this->map[0], ~this->map[1]);
    }

    constexpr bool contains(char ch) const {
        return ch < 0 ? false :
               ch < 64 ? this->map[0] & (1L << ch) :
               this->map[1] & (1L << (ch - 64));
    }
};

constexpr AsciiMap makeFromRange(AsciiMap asciiMap, char start, char stop) {
    return start < stop ? makeFromRange(asciiMap + start, start + 1, stop) : asciiMap + start;
}

constexpr bool checkCharRange(char start, char stop) {
    return start <= stop ? true : misc::constexpr_error<bool>("start character must be stop character or less");
}

constexpr AsciiMap convertToAsciiMap(const char *str, size_t size, size_t index, AsciiMap map) {
            // negate ascii map
    return index == 0 && str[0] == '^' ? ~convertToAsciiMap(str, size, index + 1, map) :

           // terminal
           index == size ? map :

           // escape '^'
           str[index] == '\\' && index + 1 < size && str[index + 1] == '^' ?
                convertToAsciiMap(str, size, index + 2, map + '^') :

           // escape '-'
           str[index] == '\\' && index + 1 < size && str[index + 1] == '-' ?
                convertToAsciiMap(str, size, index + 2, map + '-') :

           // parse character range
           index > 0 && str[index - 1] != '^' && str[index] == '-' && index + 1 < size
           && checkCharRange(str[index - 1], str[index + 1]) ?
                convertToAsciiMap(str, size, index + 2, makeFromRange(map, str[index - 1], str[index + 1])) :

           // update ascii map (default case)
           convertToAsciiMap(str, size, index + 1, map + str[index]);
}

constexpr AsciiMap convertToAsciiMap(const char *str, size_t size) {
    return convertToAsciiMap(str, size, 0, AsciiMap());
}

template <bool B>
struct Utf8Util {
    unsigned int utf8ByteSize(unsigned char b) const {
        static const unsigned char table[256] = {
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

                2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
                4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0,
        };
        return table[b];
    }
};

constexpr bool isAsciiStr(const char *str, std::size_t index, std::size_t size) {
    return index == size ? true : str[index] >= 0 && isAsciiStr(str, index + 1, size);
}

} // namespace unicode_util
} // namespace aquarius

#endif //AQUARIUS_CXX_INTERNAL_ASCII_HPP
