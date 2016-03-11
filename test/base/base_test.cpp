#include <iostream>
#include <string>

#include <gtest/gtest.h>

#include <aquarius.hpp>

template <typename P>
constexpr bool check_unit(P) {
    static_assert(aquarius::misc::is_unit<typename P::retType>::value, "must be unit type");
    return true;
}

template <typename T, typename P>
constexpr bool check_same(P) {
    static_assert(std::is_same<typename P::retType, T>::value, "must be same type");
    return true;
};


TEST(base, any) {
    using namespace aquarius;

    check_unit(ANY);

    std::string input("a");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto r = ANY(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(1, state.cursor() - state.begin());
    });

    // failed case
    input = "";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto r = ANY(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0, state.cursor() - state.begin());
    });
}

TEST(base, string1) {
    using namespace aquarius;

    constexpr auto p = "a"_str;
    check_unit(p);

    std::string input("a");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto r = p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(1, state.cursor() - state.begin());
    });

    // failed case1
    input = "w";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto r = p(state);
        ASSERT_FALSE(state.result());
         ASSERT_EQ(0, state.cursor() - state.begin());
    });

    // failed case2
    input = "";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto r = p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0, state.cursor() - state.begin());
    });
}

TEST(base, string2) {
    using namespace aquarius;

    constexpr auto p = "abc"_str;
    check_unit(p);

    std::string input("abcd");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto r = p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(3, state.cursor() - state.begin());
    });

    // failed case1
    input = "abs";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto r = p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0, state.cursor() - state.begin());
    });

    // failed case2
    input = "";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto r = p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0, state.cursor() - state.begin());
    });
}

TEST(base, charClass1) {
    using namespace aquarius;

    constexpr auto p = ch('a');
    check_unit(p);

    std::string input("abc");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto r = p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(1, state.cursor() - state.begin());
    });

    // failed case1
    input = "r";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto r = p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0, state.cursor() - state.begin());
    });

    // failed case2
    input = "";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto r = p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0, state.cursor() - state.begin());
    });
}

TEST(base, charClass2) {
    using namespace aquarius;

    constexpr auto p = ch('a', '1', 'C');
    check_unit(p);

    std::string input("1Ca");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto r = p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(1, state.cursor() - state.begin());

        r = p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(2, state.cursor() - state.begin());

        r = p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(3, state.cursor() - state.begin());
    });

    // failed case1
    input = "@";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto r = p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0, state.cursor() - state.begin());
    });

    // failed case2
    input = "";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto r = p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0, state.cursor() - state.begin());
    });
}

TEST(base, charClass3) {
    using namespace aquarius;

    constexpr auto p = ch(r('0', '9'), '_', r('a', 'z'));
    check_unit(p);

    std::string input("10_aeh9op62qz8l");
    auto state = createState(input.begin(), input.end());

    for(unsigned int i = 0; i < input.size(); i++) {
        ASSERT_NO_FATAL_FAILURE({
            auto r = p(state);
            ASSERT_TRUE(state.result());
            ASSERT_EQ(i + 1, state.cursor() - state.begin());
        });
    }

    // failed case1
    input = "+";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto r = p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0, state.cursor() - state.begin());
    });

    // failed case2
    input = "";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto r = p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0, state.cursor() - state.begin());
    });
}

TEST(base, andPredicate) {
    using namespace aquarius;

    constexpr auto p = &"abc"_str;
    check_unit(p);

    std::string input("abc");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto r = p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(0, state.cursor() - state.begin());
    });

    // failed case
    input = "ab";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto r = p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0, state.cursor() - state.begin());
    });
}

TEST(base, notPredicate) {
    using namespace aquarius;

    constexpr auto p = !"abc"_str;
    check_unit(p);

    std::string input("1234");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto r = p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(0, state.cursor() - state.begin());
    });

    // failed case
    input = "abc";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto r = p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0, state.cursor() - state.begin());
    });
}

TEST(base, capture) {
    using namespace aquarius;

    constexpr auto p = text["hello"_str];
    check_same<std::string>(p);

    std::string input("hello");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto str = p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(5, state.cursor() - state.begin());
        ASSERT_STREQ("hello", str.c_str());
    });

    // failed case
    input = "hfue";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto str = p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0, state.cursor() - state.begin());
    });
}

TEST(base, zeroMore1) {
    using namespace aquarius;

    constexpr auto p = *ch(r('A', 'Z'));
    check_unit(p);

    std::string input("ABCDEFGH");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto r = p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(8, state.cursor() - state.begin());
    });

    input = "def";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto r = p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(0, state.cursor() - state.begin());
    });
}

TEST(base, zeroMore2) {
    using namespace aquarius;

    constexpr auto p = *text[ "ABC "_str ];
    check_same<std::vector<std::string>>(p);

    std::string input("ABC ABC ABC ");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto results = p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(12, state.cursor() - state.begin());
        ASSERT_EQ(3, results.size());

        ASSERT_STREQ("ABC ", results[0].c_str());
        ASSERT_STREQ("ABC ", results[1].c_str());
        ASSERT_STREQ("ABC ", results[2].c_str());
    });

    input = "def";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto values = p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(0, state.cursor() - state.begin());
        ASSERT_EQ(0, values.size());
    });
}

TEST(base, oneMore1) {
    using namespace aquarius;

    constexpr auto p = +ch(r('A', 'Z'));
    check_unit(p);

    std::string input("ABCDEFGH");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto r = p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(8, state.cursor() - state.begin());
    });

    // failed case
    input = "def";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto r = p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0, state.cursor() - state.begin());
    });
}

TEST(base, oneMore2) {
    using namespace aquarius;

    constexpr auto p = +text[ "ABC "_str ];
    check_same<std::vector<std::string>>(p);

    std::string input("ABC ABC ABC ");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto results = p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(12, state.cursor() - state.begin());
        ASSERT_EQ(3, results.size());

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
        ASSERT_EQ(0, state.cursor() - state.begin());
        ASSERT_EQ(0, values.size());
    });
}

TEST(base, option1) {
    using namespace aquarius;

    constexpr auto p = -"hello"_str;
    check_unit(p);

    std::string input("hello");
    auto state = createState(input.begin(), input.end());

    auto result = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(5, state.cursor() - state.begin()));

    input = "hel";
    state = createState(input.begin(), input.end());

    result = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(0, state.cursor() - state.begin()));
}

TEST(base, option2) {
    using namespace aquarius;

    constexpr auto p = -text[ "world"_str ];
    check_same<Optional<std::string>>(p);

    std::string input("world   ");
    auto state = createState(input.begin(), input.end());

    auto result = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(5, state.cursor() - state.begin()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(static_cast<bool>(result)));
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(result.get() == "world"));


    input = "worl";
    state = createState(input.begin(), state.end());

    result = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(0, state.cursor() - state.begin()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_FALSE(static_cast<bool>(result)));
}

TEST(base, seq1) {
    using namespace aquarius;

    constexpr auto p = "hello"_str >> " "_str >> "world"_str;
    check_unit(p);

    std::string input("hello world");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto r = p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(11, state.cursor() - state.begin());
    });

    // failed case
    input = "fre";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto r = p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0, state.cursor() - state.begin());
    });
}

TEST(base, seq2) {
    using namespace aquarius;

    constexpr auto p = text[ "hello"_str ] >> " world"_str;
    check_same<std::string>(p);

    std::string input("hello world");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto value = p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(11, state.cursor() - state.begin());
        ASSERT_STREQ("hello", value.c_str());
    });

    // failed case
    input = "hey";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto value = p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0, state.cursor() - state.begin());
    });
}

TEST(base, seq3) {
    using namespace aquarius;

    constexpr auto p = "hello "_str >> text[ "world"_str ];
    check_same<std::string>(p);

    std::string input("hello world");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto value = p(state);
        ASSERT_TRUE(state.result());
        ASSERT_EQ(11, state.cursor() - state.begin());
        ASSERT_STREQ("world", value.c_str());
    });

    // failed case
    input = "hey";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        auto value = p(state);
        ASSERT_FALSE(state.result());
        ASSERT_EQ(0, state.cursor() - state.begin());
    });
}

TEST(base, seq4) {
    using namespace aquarius;

    constexpr auto p = text[ "hello"_str ] >> " "_str >> text[ "world"_str ];
    check_same<std::tuple<std::string, std::string>>(p);

    std::string input("hello world");
    auto state = createState(input.begin(), input.end());

    auto r = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(11, state.cursor() - state.begin()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(std::get<0>(r), "hello"));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(std::get<1>(r), "world"));

    // failure
    input = "hello0";
    state = createState(input.begin(), input.end());

    r = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_FALSE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(0, state.cursor() - state.begin()));
}

TEST(base, seq5) {
    using namespace aquarius;

    constexpr auto p = text[ "hello"_str ] >> " "_str >> text[ "world"_str ] >> -text[ "!!"_str ];
    check_same<std::tuple<std::string, std::string, Optional<std::string>>>(p);

    std::string input("hello world");
    auto state = createState(input.begin(), input.end());

    auto r = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(11, state.cursor() - state.begin()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(std::get<0>(r), "hello"));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(std::get<1>(r), "world"));
    ASSERT_NO_FATAL_FAILURE(ASSERT_FALSE(static_cast<bool>(std::get<2>(r))));

    input = "hello world!!";
    state = createState(input.begin(), input.end());

    r = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(13, state.cursor() - state.begin()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(std::get<0>(r), "hello"));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(std::get<1>(r), "world"));
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(static_cast<bool>(std::get<2>(r))));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(std::get<2>(r).get(), "!!"));

    // failure
    input = "hello0";
    state = createState(input.begin(), input.end());

    r = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_FALSE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(0, state.cursor() - state.begin()));
}

TEST(base, seq6) {
    using namespace aquarius;

    constexpr auto p = text[ "hello"_str ] >> (" "_str >> text[ "world"_str ]) >> text[ "!!"_str ];
    check_same<std::tuple<std::string, std::string, std::string>>(p);

    std::string input = "hello world!!";
    auto state = createState(input.begin(), input.end());

    auto r = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(13, state.cursor() - state.begin()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(std::get<0>(r), "hello"));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(std::get<1>(r), "world"));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(std::get<2>(r), "!!"));

    // failure
    input = "hello0";
    state = createState(input.begin(), input.end());

    r = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_FALSE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(0, state.cursor() - state.begin()));
}

TEST(base, choice) {
    using namespace aquarius;

    constexpr auto p = text[ "world"_str ] | text[ "he"_str >> "llo"_str ];
    check_same<std::string>(p);

    std::string input("hello");
    auto state = createState(input.begin(), input.end());

    auto result = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(5, state.cursor() - state.begin()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(result == "hello"));

    input = "world";
    state = createState(input.begin(), input.end());

    result = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(5, state.cursor() - state.begin()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(result == "world"));

    // failed case
    input = "worl";
    state = createState(input.begin(), input.end());

    result = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_FALSE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(0, state.cursor() - state.begin()));
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

    constexpr auto p = text[ "12"_str ] >> "+"_str >> text[ "18"_str ] && Sum();
    check_same<int>(p);

    std::string input("12+18");
    auto state = createState(input.begin(), input.end());
    auto r = p(state);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(state.result()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(5, state.cursor() - state.begin()));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(30, r));
}


namespace top {

using namespace aquarius;

AQUARIUS_DECL_RULE(unit, S);

AQUARIUS_DEFINE_RULE(unit, SPACE, *nterm<S>());

AQUARIUS_DEFINE_RULE(unit, S, ch(' ', '\t', '\n', '\r'));

AQUARIUS_DEFINE_RULE(unit, Rep, ch('b') | ch('a') >> nterm<Rep>());

}

TEST(base, nterm1) {
    using namespace top;
    using namespace aquarius;

    std::string line("  \t \n ");
    auto state = createState(line.begin(), line.end());

    auto r = SPACE::pattern()(state);
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