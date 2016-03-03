#include <iostream>

#include <gtest/gtest.h>

#include <aquarius.hpp>

#define CHECK_TYPE(p) \
static_assert(aquarius::misc::is_unit<decltype(p)::retType>::value, "must be unit type")

TEST(base, any) {
    using namespace aquarius;

    CHECK_TYPE(ANY);

    std::string input("a");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        bool r = ANY(state);
        ASSERT_TRUE(r);
        ASSERT_EQ(1, state.cursor() - state.begin());
    });

    // failed case
    input = "";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        bool r = ANY(state);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, state.cursor() - state.begin());
    });
}

TEST(base, string1) {
    using namespace aquarius;

    constexpr auto p = "a"_str;
    CHECK_TYPE(p);

    std::string input("a");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(state);
        ASSERT_TRUE(r);
        ASSERT_EQ(1, state.cursor() - state.begin());
    });

    // failed case1
    input = "w";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(state);
        ASSERT_FALSE(r);
         ASSERT_EQ(0, state.cursor() - state.begin());
    });

    // failed case2
    input = "";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(state);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, state.cursor() - state.begin());
    });
}

TEST(base, string2) {
    using namespace aquarius;

    constexpr auto p = "abc"_str;
    CHECK_TYPE(p);

    std::string input("abcd");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(state);
        ASSERT_TRUE(r);
        ASSERT_EQ(3, state.cursor() - state.begin());
    });

    // failed case1
    input = "abs";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(state);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, state.cursor() - state.begin());
    });

    // failed case2
    input = "";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(state);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, state.cursor() - state.begin());
    });
}

TEST(base, charClass1) {
    using namespace aquarius;

    constexpr auto p = ch('a');
    CHECK_TYPE(p);

    std::string input("abc");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(state);
        ASSERT_TRUE(r);
        ASSERT_EQ(1, state.cursor() - state.begin());
    });

    // failed case1
    input = "r";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(state);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, state.cursor() - state.begin());
    });

    // failed case2
    input = "";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(state);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, state.cursor() - state.begin());
    });
}

TEST(base, charClass2) {
    using namespace aquarius;

    constexpr auto p = ch('a', '1', 'C');
    CHECK_TYPE(p);

    std::string input("1Ca");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(state);
        ASSERT_TRUE(r);
        ASSERT_EQ(1, state.cursor() - state.begin());

        r = p(state);
        ASSERT_TRUE(r);
        ASSERT_EQ(2, state.cursor() - state.begin());

        r = p(state);
        ASSERT_TRUE(r);
        ASSERT_EQ(3, state.cursor() - state.begin());
    });

    // failed case1
    input = "@";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(state);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, state.cursor() - state.begin());
    });

    // failed case2
    input = "";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(state);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, state.cursor() - state.begin());
    });
}

TEST(base, charClass3) {
    using namespace aquarius;

    constexpr auto p = ch(r('0', '9'), '_', r('a', 'z'));
    CHECK_TYPE(p);

    std::string input("10_aeh9op62qz8l");
    auto state = createState(input.begin(), input.end());

    for(unsigned int i = 0; i < input.size(); i++) {
        ASSERT_NO_FATAL_FAILURE({
            bool r = p(state);
            ASSERT_TRUE(r);
            ASSERT_EQ(i + 1, state.cursor() - state.begin());
        });
    }

    // failed case1
    input = "+";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(state);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, state.cursor() - state.begin());
    });

    // failed case2
    input = "";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(state);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, state.cursor() - state.begin());
    });
}

TEST(base, andPredicate) {
    using namespace aquarius;

    constexpr auto p = &"abc"_str;
    CHECK_TYPE(p);

    std::string input("abc");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(state);
        ASSERT_TRUE(r);
        ASSERT_EQ(0, state.cursor() - state.begin());
    });

    // failed case
    input = "ab";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(state);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, state.cursor() - state.begin());
    });
}

TEST(base, notPredicate) {
    using namespace aquarius;

    constexpr auto p = !"abc"_str;
    CHECK_TYPE(p);

    std::string input("1234");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(state);
        ASSERT_TRUE(r);
        ASSERT_EQ(0, state.cursor() - state.begin());
    });

    // failed case
    input = "abc";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(state);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, state.cursor() - state.begin());
    });
}

TEST(base, capture) {
    using namespace aquarius;

    constexpr auto p = text["hello"_str];
    static_assert(std::is_same<std::string, decltype(p)::retType>::value, "must be string type");

    std::string input("hello");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        std::string str;
        bool r = p(state, str);
        ASSERT_TRUE(r);
        ASSERT_EQ(5, state.cursor() - state.begin());
        ASSERT_STREQ("hello", str.c_str());
    });

    // failed case
    input = "hfue";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        std::string str;
        bool r = p(state, str);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, state.cursor() - state.begin());
    });
}

TEST(base, zeroMore1) {
    using namespace aquarius;

    constexpr auto p = *ch(r('A', 'Z'));
    CHECK_TYPE(p);

    std::string input("ABCDEFGH");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(state);
        ASSERT_TRUE(r);
        ASSERT_EQ(8, state.cursor() - state.begin());
    });

    input = "def";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(state);
        ASSERT_TRUE(r);
        ASSERT_EQ(0, state.cursor() - state.begin());
    });
}

TEST(base, zeroMore2) {
    using namespace aquarius;

    constexpr auto p = *text[ "ABC "_str ];
    static_assert(std::is_same<std::vector<std::string>, decltype(p)::retType>::value, "same type");

    std::string input("ABC ABC ABC ");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        std::vector<std::string> results;
        bool r = p(state, results);
        ASSERT_TRUE(r);
        ASSERT_EQ(12, state.cursor() - state.begin());
        ASSERT_EQ(3, results.size());

        ASSERT_STREQ("ABC ", results[0].c_str());
        ASSERT_STREQ("ABC ", results[1].c_str());
        ASSERT_STREQ("ABC ", results[2].c_str());
    });

    input = "def";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        std::vector<std::string> values;
        bool r = p(state, values);
        ASSERT_TRUE(r);
        ASSERT_EQ(0, state.cursor() - state.begin());
        ASSERT_EQ(0, values.size());
    });
}

TEST(base, oneMore1) {
    using namespace aquarius;

    constexpr auto p = +ch(r('A', 'Z'));
    CHECK_TYPE(p);

    std::string input("ABCDEFGH");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(state);
        ASSERT_TRUE(r);
        ASSERT_EQ(8, state.cursor() - state.begin());
    });

    // failed case
    input = "def";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(state);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, state.cursor() - state.begin());
    });
}

TEST(base, oneMore2) {
    using namespace aquarius;

    constexpr auto p = +text[ "ABC "_str ];
    static_assert(std::is_same<std::vector<std::string>, decltype(p)::retType>::value, "same type");

    std::string input("ABC ABC ABC ");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        std::vector<std::string> results;
        bool r = p(state, results);
        ASSERT_TRUE(r);
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
        std::vector<std::string> values;
        bool r = p(state, values);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, state.cursor() - state.begin());
        ASSERT_EQ(0, values.size());
    });
}

TEST(base, seq1) {
    using namespace aquarius;

    constexpr auto p = "hello"_str >> " "_str >> "world"_str;
    CHECK_TYPE(p);

    std::string input("hello world");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(state);
        ASSERT_TRUE(r);
        ASSERT_EQ(11, state.cursor() - state.begin());
    });

    // failed case
    input = "fre";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(state);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, state.cursor() - state.begin());
    });
}

TEST(base, seq2) {
    using namespace aquarius;

    constexpr auto p = text[ "hello"_str ] >> " world"_str;
    static_assert(std::is_same<decltype(p)::retType, std::string>::value, "must be same type");

    std::string input("hello world");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        std::string value;
        bool r = p(state, value);
        ASSERT_TRUE(r);
        ASSERT_EQ(11, state.cursor() - state.begin());
        ASSERT_STREQ("hello", value.c_str());
    });

    // failed case
    input = "hey";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        std::string value;
        bool r = p(state, value);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, state.cursor() - state.begin());
    });
}

TEST(base, seq3) {
    using namespace aquarius;

    constexpr auto p = "hello "_str >> text[ "world"_str ];
    static_assert(std::is_same<decltype(p)::retType, std::string>::value, "must be same type");

    std::string input("hello world");
    auto state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        std::string value;
        bool r = p(state, value);
        ASSERT_TRUE(r);
        ASSERT_EQ(11, state.cursor() - state.begin());
        ASSERT_STREQ("world", value.c_str());
    });

    // failed case
    input = "hey";
    state = createState(input.begin(), input.end());

    ASSERT_NO_FATAL_FAILURE({
        std::string value;
        bool r = p(state, value);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, state.cursor() - state.begin());
    });
}

namespace top {

using namespace aquarius;


struct S : AQUARIUS_RHS(unit, ch(' ', '\t', '\n', '\r'));

AQUARIUS_DEFINE_RULE(unit, SPACE, *nonTerm<S>());

}

//TEST(base, nterm) {
//    using namespace top;
//    using namespace aquarius;
//
//    std::string line("  \t \n ");
//    auto begin = line.begin();
//    auto end = line.end();
//
//    bool r = SPACE::pattern()(begin, end);
//    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(r));
//    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(6, begin - line.begin()));
//
//    auto rr = Parser<SPACE>()(line.begin(), line.end());
//    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(static_cast<bool>(rr)));
//    ASSERT_NO_FATAL_FAILURE(ASSERT_FALSE(rr.hasResult()));
//}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}