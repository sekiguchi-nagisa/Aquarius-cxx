#include <iostream>
#include <string>

#include "gtest/gtest.h"

#include <aquarius.hpp>

template <typename P>
constexpr bool check_unit(P) {
    static_assert(std::is_void<typename P::retType>::value, "must be void type");
    return true;
}

template <typename T, typename P>
constexpr bool check_same(P) {
    static_assert(std::is_same<typename P::retType, T>::value, "must be same type");
    return true;
}

TEST(base, any1) {
    using namespace aquarius;
    using namespace aquarius::ascii;

    check_unit(ANY);

    std::string input("a");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        ANY(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(1u, state.consumedSize());
    });

    // failed case1
    input = "";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        ANY(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0u, state.consumedSize());
    });

    // failed case2
    char b[] = {-43, -45, -9, 0};
    input = b;
    state = createState(input.begin(), input.end());

    ANY(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_FALSE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(0u, state.consumedSize()));
}

TEST(base, any2) {
    using namespace aquarius;
    using namespace aquarius::ascii;

    std::string input;
    {
        char buf[] = {static_cast<char>(0xE3), static_cast<char>(0x81),
                      static_cast<char>(0x82), static_cast<char>(0x00)};  // 'あ'
        input = std::string(buf);
    }
    auto state = createState(input.begin(), input.end());

    unicode::ANY(state);

    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(3u, state.consumedSize()));

    // failed case
    {
        char buf[] = {static_cast<char>(243), static_cast<char>(23), static_cast<char>(0x00)};
        input = std::string(buf);
    }
    state = createState(input.begin(), input.end());

    unicode::ANY(state);

    ASSERT_NO_FATAL_FAILURE(ASSERT_FALSE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(0u, state.consumedSize()));
}


TEST(base, string1) {
    using namespace aquarius;
    using namespace aquarius::ascii;

    constexpr auto p = str("a");
    check_unit(p);

    std::string input("a");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(1u, state.consumedSize());
    });

    // failed case1
    input = "w";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        p(state);
        ASSERT_FALSE(state.result());
         ASSERT_EQ(0u, state.consumedSize());
    });

    // failed case2
    input = "";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0u, state.consumedSize());
    });
}

TEST(base, string2) {
    using namespace aquarius;
    using namespace ascii;

    constexpr auto p = str("abc");
    check_unit(p);

    std::string input("abcd");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(3u, state.consumedSize());
    });

    // failed case1
    input = "abs";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0, state.cursor() - state.begin());
    });

    // failed case2
    input = "";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0u, state.consumedSize());
    });
}

TEST(base, string3) {
    using namespace aquarius;
    using namespace unicode;

    aquarius_pattern_t p = str("あい2う");
    check_unit(p);

    std::string input("あい2う");
    auto state = createState(input.begin(), input.end());

    p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(10u, state.consumedSize()));

    // failed case
    input = "あい";
    state = createState(input.begin(), input.end());

    p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_FALSE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(0u, state.consumedSize()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(0u, state.failurePos()));

    // failed case2
    input = "あい3え";
    state = createState(input.begin(), input.end());

    p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_FALSE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(0u, state.consumedSize()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(6u, state.failurePos()));
}

TEST(base, charClass1) {
    using namespace aquarius;

    constexpr auto p = ch('a');
    check_unit(p);

    std::string input("abc");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(1u, state.consumedSize());
    });

    // failed case1
    input = "r";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0u, state.consumedSize());
    });

    // failed case2
    input = "";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0u, state.consumedSize());
    });
}

TEST(base, charClass2) {
    using namespace aquarius;

    constexpr auto p = set("a1C");
    check_unit(p);

    std::string input("1Ca");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(1u, state.consumedSize());

        p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(2u, state.consumedSize());

        p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(3u, state.consumedSize());
    });

    // failed case1
    input = "@";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0u, state.consumedSize());
    });

    // failed case2
    input = "";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0u, state.consumedSize());
    });
}

TEST(base, charClass3) {
    using namespace aquarius;

    constexpr auto p = set("0-9_a-z");
    check_unit(p);

    std::string input("10_aeh9op62qz8l");
    auto state = createState(input.begin(), input.end());

    for(unsigned int i = 0; i < input.size(); i++) {
        ASSERT_NO_FATAL_FAILURE({
            p(state);
            ASSERT_TRUE(state.result());
            ASSERT_EQ(i + 1, state.consumedSize());
        });
    }

    // failed case1
    input = "+";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0u, state.consumedSize());
    });

    // failed case2
    input = "";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0u, state.consumedSize());
    });
}

TEST(base, charClass4) {
    using namespace aquarius;
    using namespace unicode;

    constexpr auto p = ch(U'あ');
    check_unit(p);

    std::string input("あ");
    auto state = createState(input.begin(), input.end());

    p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(3u, state.consumedSize()));

    // failed
    input = "い";
    state = createState(input.begin(), input.end());

    p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_FALSE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(0u, state.consumedSize()));
}

TEST(base, charClass5) {
    using namespace aquarius;
    using namespace unicode;

    constexpr auto p = set(U"３");
    check_unit(p);

    std::string input("３");
    auto state = createState(input.begin(), input.end());

    p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(3u, state.consumedSize()));

    // failed
    input = "い";
    state = createState(input.begin(), input.end());

    p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_FALSE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(0u, state.consumedSize()));
}

TEST(base, charClass6) {
    using namespace aquarius;
    using namespace unicode;

    constexpr auto p = set(U"4");
    check_unit(p);

    std::string input("4");
    auto state = createState(input.begin(), input.end());

    p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(1u, state.consumedSize()));

    // failed
    input = "い";
    state = createState(input.begin(), input.end());

    p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_FALSE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(0u, state.consumedSize()));
}

TEST(base, charClass7) {
    using namespace aquarius;
    using namespace unicode;

    constexpr auto p = set(U"３う1-5");
    check_unit(p);

    std::string input("３");
    auto state = createState(input.begin(), input.end());

    p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(3u, state.consumedSize()));

    input = "う";
    state = createState(input.begin(), input.end());

    p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(3u, state.consumedSize()));

    input = "5";
    state = createState(input.begin(), input.end());

    p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(1u, state.consumedSize()));

    input = "1";
    state = createState(input.begin(), input.end());

    p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(1u, state.consumedSize()));

    input = "2";
    state = createState(input.begin(), input.end());

    p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(1u, state.consumedSize()));

    // failed
    input = "け";
    state = createState(input.begin(), input.end());

    p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_FALSE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(0u, state.consumedSize()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(0u, state.failurePos()));
}

TEST(base, andPredicate) {
    using namespace aquarius;
    using namespace ascii;

    constexpr auto p = ~str("abc");
    check_unit(p);

    std::string input("abc");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(0u, state.consumedSize());
    });

    // failed case
    input = "ab";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0u, state.consumedSize());
    });
}

TEST(base, notPredicate) {
    using namespace aquarius;
    using namespace ascii;

    constexpr auto p = !str("abc");
    check_unit(p);

    std::string input("1234");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(0u, state.consumedSize());
    });

    // failed case
    input = "abc";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0u, state.consumedSize());
    });
}

TEST(base, capture) {
    using namespace aquarius;
    using namespace ascii;

    constexpr auto p = text[str("hello")];
    check_same<std::string>(p);

    std::string input("hello");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto str = p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(5u, state.consumedSize());
        ASSERT_STREQ("hello", str.c_str());
    });

    // failed case
    input = "hfue";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto str = p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0u, state.consumedSize());
    });
}

TEST(base, zeroMore1) {
    using namespace aquarius;

    constexpr auto p = *set("A-Z");
    check_unit(p);

    std::string input("ABCDEFGH");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(8u, state.consumedSize());
    });

    input = "def";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(0u, state.consumedSize());
    });
}

TEST(base, zeroMore2) {
    using namespace aquarius;
    using namespace ascii;

    constexpr auto p = *text[ str("ABC ") ];
    check_same<std::vector<std::string>>(p);

    std::string input("ABC ABC ABC ");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto results = p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(12u, state.consumedSize());
        ASSERT_EQ(3u, results.size());

        ASSERT_STREQ("ABC ", results[0].c_str());
        ASSERT_STREQ("ABC ", results[1].c_str());
        ASSERT_STREQ("ABC ", results[2].c_str());
    });

    input = "def";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto values = p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(0u, state.consumedSize());
        ASSERT_EQ(0u, values.size());
    });
}

TEST(base, oneMore1) {
    using namespace aquarius;

    constexpr auto p = +set("A-Z");
    check_unit(p);

    std::string input("ABCDEFGH");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(8u, state.consumedSize());
    });

    // failed case
    input = "def";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0u, state.consumedSize());
    });
}

TEST(base, oneMore2) {
    using namespace aquarius;
    using namespace ascii;

    constexpr auto p = +text[ str("ABC ") ];
    check_same<std::vector<std::string>>(p);

    std::string input("ABC ABC ABC ");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto results = p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(12u, state.consumedSize());
        ASSERT_EQ(3u, results.size());

        ASSERT_STREQ("ABC ", results[0].c_str());
        ASSERT_STREQ("ABC ", results[1].c_str());
        ASSERT_STREQ("ABC ", results[2].c_str());
    });

    // failed case
    input = "def";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto values = p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0u, state.consumedSize());
        ASSERT_EQ(0u, values.size());
    });
}

TEST(base, repeat1) {
    using namespace aquarius;

    constexpr auto p = repeat<2, 4>(ch('a'), *ch(' '));
    check_unit(p);

    std::string input("a a a a");
    auto state = createState(input.begin(), input.end());

    p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(input.size(), state.consumedSize()));

    // failed case1
    input = "a a a a a";
    state = createState(input.begin(), input.end());

    p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(input.size() - 2, state.consumedSize()));

    // failed case2
    input = "a ";
    state = createState(input.begin(), input.end());

    p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_FALSE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(2u, state.consumedSize()));
}

TEST(base, repeat2) {
    using namespace aquarius;

    constexpr auto p = repeat<2, 4>(text [ ch('a') ], *ch(' '));
    check_same<std::vector<std::string>>(p);

    std::string input("a a a a");
    auto state = createState(input.begin(), input.end());

    auto r = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(input.size(), state.consumedSize()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(4u, r.size()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE("a" == r[0]));
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE("a" == r[1]));
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE("a" == r[2]));
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE("a" == r[3]));

    // failed case1
    input = "a a a a a";
    state = createState(input.begin(), input.end());

    r = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(input.size() - 2, state.consumedSize()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(4u, r.size()));

    // failed case2
    input = "a ";
    state = createState(input.begin(), input.end());

    r = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_FALSE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(2u, state.consumedSize()));
}

TEST(base, option1) {
    using namespace aquarius;
    using namespace ascii;

    constexpr auto p = -str("hello");
    check_unit(p);

    std::string input("hello");
    auto state = createState(input.begin(), input.end());

    p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(5u, state.consumedSize()));

    input = "hel";
    state = createState(input.begin(), input.end());

    p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(0u, state.consumedSize()));
}

TEST(base, option2) {
    using namespace aquarius;
    using namespace ascii;

    constexpr auto p = -text[ str("world") ];
    check_same<Optional<std::string>>(p);

    std::string input("world   ");
    auto state = createState(input.begin(), input.end());

    auto result = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(5u, state.consumedSize()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(static_cast<bool>(result)));
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(result.get() == "world"));


    input = "worl";
    state = createState(input.begin(), state.end());

    result = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(0u, state.consumedSize()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_FALSE(static_cast<bool>(result)));
}

TEST(base, seq1) {
    using namespace aquarius;
    using namespace ascii;

    constexpr auto p = str("hello") >> str(" ") >> str("world");
    check_unit(p);

    std::string input("hello world");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(11u, state.consumedSize());
    });

    // failed case
    input = "fre";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0u, state.consumedSize());
    });
}

TEST(base, seq2) {
    using namespace aquarius;
    using namespace ascii;

    constexpr auto p = text[ str("hello") ] >> str(" world");
    check_same<std::string>(p);

    std::string input("hello world");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto value = p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(11u, state.consumedSize());
        ASSERT_STREQ("hello", value.c_str());
    });

    // failed case
    input = "hey";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto value = p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0u, state.consumedSize());
    });
}

TEST(base, seq3) {
    using namespace aquarius;
    using namespace ascii;

    constexpr auto p = str("hello ") >> text[ str("world") ];
    check_same<std::string>(p);

    std::string input("hello world");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto value = p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(11u, state.consumedSize());
        ASSERT_STREQ("world", value.c_str());
    });

    // failed case
    input = "hey";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto value = p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0u, state.consumedSize());
    });
}

TEST(base, seq4) {
    using namespace aquarius;
    using namespace ascii;

    constexpr auto p = text[ str("hello") ] >> str(" ") >> text[ str("world") ];
    check_same<std::tuple<std::string, std::string>>(p);

    std::string input("hello world");
    auto state = createState(input.begin(), input.end());

    auto r = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(11u, state.consumedSize()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(std::get<0>(r), "hello"));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(std::get<1>(r), "world"));

    // failure
    input = "hello0";
    state = createState(input.begin(), input.end());

    r = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_FALSE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(0u, state.consumedSize()));
}

TEST(base, seq5) {
    using namespace aquarius;
    using namespace ascii;

    constexpr auto p = text[ str("hello") ] >> str(" ") >> text[ str("world") ] >> -text[ str("!!") ];
    check_same<std::tuple<std::string, std::string, Optional<std::string>>>(p);

    std::string input("hello world");
    auto state = createState(input.begin(), input.end());

    auto r = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(11u, state.consumedSize()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(std::get<0>(r), "hello"));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(std::get<1>(r), "world"));
    ASSERT_NO_FATAL_FAILURE(ASSERT_FALSE(static_cast<bool>(std::get<2>(r))));

    input = "hello world!!";
    state = createState(input.begin(), input.end());

    r = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(13u, state.consumedSize()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(std::get<0>(r), "hello"));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(std::get<1>(r), "world"));
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(static_cast<bool>(std::get<2>(r))));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(std::get<2>(r).get(), "!!"));

    // failure
    input = "hello0";
    state = createState(input.begin(), input.end());

    r = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_FALSE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(0u, state.consumedSize()));
}

TEST(base, seq6) {
    using namespace aquarius;
    using namespace ascii;

    constexpr auto p = text[ str("hello") ] >> (str(" ") >> text[ str("world") ]) >> text[ str("!!") ];
    check_same<std::tuple<std::string, std::string, std::string>>(p);

    std::string input = "hello world!!";
    auto state = createState(input.begin(), input.end());

    auto r = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(13u, state.consumedSize()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(std::get<0>(r), "hello"));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(std::get<1>(r), "world"));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(std::get<2>(r), "!!"));

    // failure
    input = "hello0";
    state = createState(input.begin(), input.end());

    r = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_FALSE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(0u, state.consumedSize()));
}

TEST(base, choice) {
    using namespace aquarius;
    using namespace ascii;

    constexpr auto p = text[ str("world") ] | text[ str("he") >> str("llo") ];
    check_same<std::string>(p);

    std::string input("hello");
    auto state = createState(input.begin(), input.end());

    auto result = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(5u, state.consumedSize()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(result == "hello"));

    input = "world";
    state = createState(input.begin(), input.end());

    result = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(5u, state.consumedSize()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(result == "world"));

    // failed case
    input = "worl";
    state = createState(input.begin(), input.end());

    result = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_FALSE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(0u, state.consumedSize()));
}

struct Sum {
    int operator()(std::string &&a, std::string &&b) const {
        int x = std::stoi(a);
        int y = std::stoi(b);
        return x + y;
    }
};

TEST(base, mapper) {
    using namespace aquarius;
    using namespace ascii;

    constexpr auto p = text[ str("12") ] >> str("+") >> text[ str("18") ] >> map<Sum>();
    check_same<int>(p);

    std::string input("12+18");
    auto state = createState(input.begin(), input.end());
    auto r = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(5u, state.consumedSize()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(30, r));
}

TEST(base, mapper2) {
    using namespace aquarius;
    using namespace ascii;

    constexpr auto p = text[ str("12") ] >> str("+") >> text[ str("18") ] >> map_c<Sum>;
    check_same<int>(p);

    std::string input("12+18");
    auto state = createState(input.begin(), input.end());
    auto r = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(5u, state.consumedSize()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(30, r));
}

TEST(base, constructor) {
    using namespace aquarius;
    using namespace ascii;

    constexpr auto p = ANY >> construct<std::string>();
    check_same<std::string>(p);

    std::string input("3");
    auto state = createState(input.begin(), input.end());
    auto r = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(r.empty()));
}

TEST(base, constructor2) {
    using namespace aquarius;
    using namespace ascii;

    constexpr auto p = ANY >> cons_c<std::string>;
    check_same<std::string>(p);

    std::string input("3");
    auto state = createState(input.begin(), input.end());
    auto r = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(r.empty()));
}

TEST(base, supply) {
    using namespace aquarius;
    using namespace ascii;

    constexpr auto p = ANY >> supply(true);
    check_same<bool>(p);

    std::string input("4");
    auto state = createState(input.begin(), input.end());
    auto r = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(r));
}

TEST(base, supplyNull) {
    using namespace aquarius;
    using namespace ascii;

    constexpr auto p = ANY >> supplyNull<int>();
    check_same<std::unique_ptr<int>>(p);

    std::string input("4");
    auto state = createState(input.begin(), input.end());
    auto r = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(r.get() == nullptr));
}


struct StrJoiner {
    void operator()(std::string &str, std::string &&v) const {
        str += std::move(v);
    }
};

TEST(base, join) {
    using namespace aquarius;

    constexpr auto p = text[ ch('a') ] >> *ch(' ') >> join<StrJoiner>(text[ ch('b') ]);
    check_same<std::string>(p);

    std::string input("a     b");
    auto state = createState(input.begin(), input.end());
    auto r = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(input.size(), state.consumedSize()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(r == "ab"));
}

TEST(base, join_each1) {
    using namespace aquarius;

    constexpr auto p = text[ ch('a') ] >> *ch(' ') >> join_each0<StrJoiner>(text[ ch('b') ], *ch(' '));
    check_same<std::string>(p);

    std::string input("a     b    b    b");
    auto state = createState(input.begin(), input.end());
    auto r = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(input.size(), state.consumedSize()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(r == "abbb"));
}

TEST(base, join_each2) {
    using namespace aquarius;
    using namespace ascii;

    constexpr auto p = text[ ch('a') ] >> *ch(' ') >> join_each0<StrJoiner>(text[ str("hello") ]);
    check_same<std::string>(p);

    std::string input("a     hellohellohello");
    auto state = createState(input.begin(), input.end());
    auto r = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(input.size(), state.consumedSize()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(r == "ahellohellohello"));
}


namespace top {

using namespace aquarius;

AQ_DECL_RULE(S, void);

AQ_DEFINE_RULE(SPACE, void) {
    return *nterm<S>();
}

AQ_DEFINE_RULE(S, void) {
    return set(" \t\n\r");
}

AQ_DEFINE_RULE(Rep, void) {
    return ch('b') | ch('a') >> nterm<Rep>();
}

}

TEST(base, nterm1) {
    using namespace top;
    using namespace aquarius;

    std::string line("  \t \n ");
    auto state = createState(line.begin(), line.end());

    SPACE::pattern()(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(6, state.cursor() - state.begin()));

    auto rr = Parser<SPACE>()(line.begin(), line.end());
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(static_cast<bool>(rr)));
}

TEST(base, nterm2) {
    using namespace top;
    using namespace aquarius;

    std::string input("aaaab");
    auto r = Parser<Rep>()(input.begin(), input.end());
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(static_cast<bool>(r)));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
