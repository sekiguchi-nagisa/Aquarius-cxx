
#include <gtest/gtest.h>

#include "internal/unicode.hpp"

using namespace aquarius;
using namespace unicode_util;

template <size_t N>
static AsciiMap createMap(char (&bits)[N]) {
    std::uint64_t low = 0;
    std::uint64_t high = 0;

    for(char ch : bits) {
        if(ch < 0) {
            fprintf(stderr, "not negate\n");
            abort();
        }

        if(ch >= 0 && ch < 64) {
            low |= (1L << ch);
        } else {
            high |= (1L << (ch - 64));
        }
    }
    return AsciiMap(low, high);
}

template <size_t N>
static constexpr AsciiMap convertToAsciiMap(const char (&str)[N]) {
    return convertToAsciiMap(str, N - 1);
}


TEST(AsciiTest, base) {
    char set[] = {'a', 'b'};
    AsciiMap map = createMap(set);

    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(map.contains('a')));
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(map.contains('b')));
    ASSERT_NO_FATAL_FAILURE(ASSERT_FALSE(map.contains('d')));
}

TEST(AsciiTest, parse1) {
    char set[] = {'a', 'b', 'c'};
    auto expect = createMap(set);

    constexpr auto map = convertToAsciiMap("abc");

    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(expect.map[0], map.map[0]));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(expect.map[1], map.map[1]));
}

TEST(AsciiTest, parse2) {
    char set[] = {'a', 'b', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    auto expect = createMap(set);

    constexpr auto map = convertToAsciiMap("a0-9b");

    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(expect.map[0], map.map[0]));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(expect.map[1], map.map[1]));
}

TEST(AsciiTest, parse3) {
    char set[] = {'a', 'b', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    auto expect = createMap(set);

    constexpr auto map = convertToAsciiMap("0-9a-b");

    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(expect.map[0], map.map[0]));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(expect.map[1], map.map[1]));
}

TEST(AsciiTest, parse4) {
    char set[] = {'a', 'b', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    auto expect = createMap(set);

    constexpr auto map = convertToAsciiMap("ba0-9");

    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(expect.map[0], map.map[0]));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(expect.map[1], map.map[1]));
}

TEST(AsciiTest, parse5) {
    char set[] = {'[', '^', ']','\\'};
    auto expect = createMap(set);

    constexpr auto map = convertToAsciiMap("[\\-^");    // cannot escape '-'

    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(expect.map[0], map.map[0]));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(expect.map[1], map.map[1]));
}

TEST(AsciiTest, parse6) {
    char set[] = {'a', 'b', '-'};
    auto expect = createMap(set);

    constexpr auto map = convertToAsciiMap("-ab");

    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(expect.map[0], map.map[0]));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(expect.map[1], map.map[1]));
}

TEST(AsciiTest, parse7) {
    char set[] = {'a', 'b', '-', '\\'};
    auto expect = createMap(set);

    constexpr auto map = convertToAsciiMap("\\ab-");

    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(expect.map[0], map.map[0]));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(expect.map[1], map.map[1]));
}

TEST(AsciiTest, parse8) {
    char set[] = {'a', 'c', '\0'};
    auto expect = createMap(set);

    constexpr auto map = convertToAsciiMap("a\0c");

    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(expect.map[0], map.map[0]));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(expect.map[1], map.map[1]));
}

TEST(AsciiTest, parse11) {
    char set[] = {'a', 'c', '^'};
    auto expect = createMap(set);

    constexpr auto map = convertToAsciiMap("^ac");

    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(expect.map[0], map.map[0]));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(expect.map[1], map.map[1]));
}

TEST(AsciiTest, parse12) {
    char set[] = {'a', 'c', '^'};
    auto expect = createMap(set);

    constexpr auto map = convertToAsciiMap("a^c");

    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(expect.map[0], map.map[0]));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(expect.map[1], map.map[1]));
}

TEST(AsciiTest, parse13) {
    char set[] = {'a', 'c', '^'};
    auto expect = createMap(set);

    constexpr auto map = convertToAsciiMap("ac^");

    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(expect.map[0], map.map[0]));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(expect.map[1], map.map[1]));
}

TEST(AsciiTest, parse15) {
    char set[] = {'^'};
    auto expect = createMap(set);

    constexpr auto map = convertToAsciiMap("^");

    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(expect.map[0], map.map[0]));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(expect.map[1], map.map[1]));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
