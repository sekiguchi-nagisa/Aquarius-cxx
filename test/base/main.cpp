#include <iostream>

#include <gtest/gtest.h>

#include <aquarius.hpp>


TEST(base, any) {
    using namespace aquarius;

    static_assert(std::is_void<decltype(ANY)::retType>::value, "must be void type");

    std::string input("a");
    auto begin = input.begin();
    auto end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        bool r = ANY(begin, end);
        ASSERT_TRUE(r);
        ASSERT_EQ(1, begin - input.begin());
    });

    // failed case
    input = "";
    begin = input.begin();
    end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        bool r = ANY(begin, end);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, begin - input.begin());
    });
}

TEST(base, string1) {
    using namespace aquarius;

    constexpr auto p = "a"_str;
    static_assert(std::is_void<decltype(p)::retType>::value, "must be void type");

    std::string input("a");
    auto begin = input.begin();
    auto end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(begin, end);
        ASSERT_TRUE(r);
        ASSERT_EQ(1, begin - input.begin());
    });

    // failed case1
    input = "w";
    begin = input.begin();
    end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(begin, end);
        ASSERT_FALSE(r);
         ASSERT_EQ(0, begin - input.begin());
    });

    // failed case2
    input = "";
    begin = input.begin();
    end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(begin, end);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, begin - input.begin());
    });
}

TEST(base, string2) {
    using namespace aquarius;

    constexpr auto p = "abc"_str;
    static_assert(std::is_void<decltype(p)::retType>::value, "must be void type");

    std::string input("abcd");
    auto begin = input.begin();
    auto end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(begin, end);
        ASSERT_TRUE(r);
        ASSERT_EQ(3, begin - input.begin());
    });

    // failed case1
    input = "abs";
    begin = input.begin();
    end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(begin, end);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, begin - input.begin());
    });

    // failed case2
    input = "";
    begin = input.begin();
    end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(begin, end);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, begin - input.begin());
    });
}

TEST(base, charClass1) {
    using namespace aquarius;

    constexpr auto p = ch('a');
    static_assert(std::is_void<decltype(p)::retType>::value, "must be void type");

    std::string input("abc");
    auto begin = input.begin();
    auto end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(begin, end);
        ASSERT_TRUE(r);
        ASSERT_EQ(1, begin - input.begin());
    });

    // failed case1
    input = "r";
    begin = input.begin();
    end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(begin, end);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, begin - input.begin());
    });

    // failed case2
    input = "";
    begin = input.begin();
    end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(begin, end);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, begin - input.begin());
    });
}

TEST(base, charClass2) {
    using namespace aquarius;

    constexpr auto p = ch('a', '1', 'C');
    static_assert(std::is_void<decltype(p)::retType>::value, "must be void type");

    std::string input("1Ca");
    auto begin = input.begin();
    auto end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(begin, end);
        ASSERT_TRUE(r);
        ASSERT_EQ(1, begin - input.begin());

        r = p(begin, end);
        ASSERT_TRUE(r);
        ASSERT_EQ(2, begin - input.begin());

        r = p(begin, end);
        ASSERT_TRUE(r);
        ASSERT_EQ(3, begin - input.begin());
    });

    // failed case1
    input = "@";
    begin = input.begin();
    end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(begin, end);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, begin - input.begin());
    });

    // failed case2
    input = "";
    begin = input.begin();
    end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(begin, end);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, begin - input.begin());
    });
}

TEST(base, charClass3) {
    using namespace aquarius;

    constexpr auto p = ch(r('0', '9'), '_', r('a', 'z'));
    static_assert(std::is_void<decltype(p)::retType>::value, "must be void type");

    std::string input("10_aeh9op62qz8l");
    auto begin = input.begin();
    auto end = input.end();

    for(unsigned int i = 0; i < input.size(); i++) {
        ASSERT_NO_FATAL_FAILURE({
            bool r = p(begin, end);
            ASSERT_TRUE(r);
            ASSERT_EQ(i + 1, begin - input.begin());
        });
    }

    // failed case1
    input = "+";
    begin = input.begin();
    end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(begin, end);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, begin - input.begin());
    });

    // failed case2
    input = "";
    begin = input.begin();
    end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(begin, end);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, begin - input.begin());
    });
}

TEST(base, andPredicate) {
    using namespace aquarius;

    constexpr auto p = &"abc"_str;
    static_assert(std::is_void<decltype(p)::retType>::value, "must be void type");

    std::string input("abc");
    auto begin = input.begin();
    auto end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(begin, end);
        ASSERT_TRUE(r);
        ASSERT_EQ(0, begin - input.begin());
    });

    // failed case
    input = "ab";
    begin = input.begin();
    end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(begin, end);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, begin - input.begin());
    });
}

TEST(base, notPredicate) {
    using namespace aquarius;

    constexpr auto p = !"abc"_str;
    static_assert(std::is_void<decltype(p)::retType>::value, "must be void type");

    std::string input("1234");
    auto begin = input.begin();
    auto end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(begin, end);
        ASSERT_TRUE(r);
        ASSERT_EQ(0, begin - input.begin());
    });

    // failed case
    input = "abc";
    begin = input.begin();
    end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(begin, end);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, begin - input.begin());
    });
}

TEST(base, capture) {
    using namespace aquarius;

    constexpr auto p = text["hello"_str];
    static_assert(std::is_same<std::string, decltype(p)::retType>::value, "must be string type");

    std::string input("hello");
    auto begin = input.begin();
    auto end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        std::string str;
        bool r = p(begin, end, str);
        ASSERT_TRUE(r);
        ASSERT_EQ(5, begin - input.begin());
        ASSERT_STREQ("hello", str.c_str());
    });

    // failed case
    input = "hfue";
    begin = input.begin();
    end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        std::string str;
        bool r = p(begin, end, str);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, begin - input.begin());
    });
}

TEST(base, zeroMore1) {
    using namespace aquarius;

    constexpr auto p = *ch(r('A', 'Z'));
    static_assert(std::is_void<decltype(p)::retType>::value, "must be void type");

    std::string input("ABCDEFGH");
    auto begin = input.begin();
    auto end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(begin, end);
        ASSERT_TRUE(r);
        ASSERT_EQ(8, begin - input.begin());
    });

    input = "def";
    begin = input.begin();
    end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(begin, end);
        ASSERT_TRUE(r);
        ASSERT_EQ(0, begin - input.begin());
    });
}

TEST(base, zeroMore2) {
    using namespace aquarius;

    constexpr auto p = *text[ "ABC "_str ];
    static_assert(std::is_same<std::vector<std::string>, decltype(p)::retType>::value, "same type");

    std::string input("ABC ABC ABC ");
    auto begin = input.begin();
    auto end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        std::vector<std::string> results;
        bool r = p(begin, end, results);
        ASSERT_TRUE(r);
        ASSERT_EQ(12, begin - input.begin());
        ASSERT_EQ(3, results.size());

        ASSERT_STREQ("ABC ", results[0].c_str());
        ASSERT_STREQ("ABC ", results[1].c_str());
        ASSERT_STREQ("ABC ", results[2].c_str());
    });

    input = "def";
    begin = input.begin();
    end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        std::vector<std::string> values;
        bool r = p(begin, end, values);
        ASSERT_TRUE(r);
        ASSERT_EQ(0, begin - input.begin());
        ASSERT_EQ(0, values.size());
    });
}

TEST(base, oneMore1) {
    using namespace aquarius;

    constexpr auto p = +ch(r('A', 'Z'));
    static_assert(std::is_void<decltype(p)::retType>::value, "must be void type");

    std::string input("ABCDEFGH");
    auto begin = input.begin();
    auto end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(begin, end);
        ASSERT_TRUE(r);
        ASSERT_EQ(8, begin - input.begin());
    });

    // failed case
    input = "def";
    begin = input.begin();
    end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(begin, end);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, begin - input.begin());
    });
}

TEST(base, oneMore2) {
    using namespace aquarius;

    constexpr auto p = +text[ "ABC "_str ];
    static_assert(std::is_same<std::vector<std::string>, decltype(p)::retType>::value, "same type");

    std::string input("ABC ABC ABC ");
    auto begin = input.begin();
    auto end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        std::vector<std::string> results;
        bool r = p(begin, end, results);
        ASSERT_TRUE(r);
        ASSERT_EQ(12, begin - input.begin());
        ASSERT_EQ(3, results.size());

        ASSERT_STREQ("ABC ", results[0].c_str());
        ASSERT_STREQ("ABC ", results[1].c_str());
        ASSERT_STREQ("ABC ", results[2].c_str());
    });

    // failed case
    input = "def";
    begin = input.begin();
    end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        std::vector<std::string> values;
        bool r = p(begin, end, values);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, begin - input.begin());
        ASSERT_EQ(0, values.size());
    });
}

TEST(base, seq1) {
    using namespace aquarius;

    constexpr auto p = "hello"_str >> " "_str >> "world"_str;
    static_assert(std::is_void<decltype(p)::retType>::value, "must be void type");

    std::string input("hello world");
    auto begin = input.begin();
    auto end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(begin, end);
        ASSERT_TRUE(r);
        ASSERT_EQ(11, begin - input.begin());
    });

    // failed case
    input = "fre";
    begin = input.begin();
    end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        bool r = p(begin, end);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, begin - input.begin());
    });
}

TEST(base, seq2) {
    using namespace aquarius;

    constexpr auto p = text[ "hello"_str ] >> " world"_str;
    static_assert(std::is_same<decltype(p)::retType, std::string>::value, "must be same type");

    std::string input("hello world");
    auto begin = input.begin();
    auto end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        std::string value;
        bool r = p(begin, end, value);
        ASSERT_TRUE(r);
        ASSERT_EQ(11, begin - input.begin());
        ASSERT_STREQ("hello", value.c_str());
    });

    // failed case
    input = "hey";
    begin = input.begin();
    end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        std::string value;
        bool r = p(begin, end, value);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, begin - input.begin());
    });
}

TEST(base, seq3) {
    using namespace aquarius;

    constexpr auto p = "hello "_str >> text[ "world"_str ];
    static_assert(std::is_same<decltype(p)::retType, std::string>::value, "must be same type");

    std::string input("hello world");
    auto begin = input.begin();
    auto end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        std::string value;
        bool r = p(begin, end, value);
        ASSERT_TRUE(r);
        ASSERT_EQ(11, begin - input.begin());
        ASSERT_STREQ("world", value.c_str());
    });

    // failed case
    input = "hey";
    begin = input.begin();
    end = input.end();

    ASSERT_NO_FATAL_FAILURE({
        std::string value;
        bool r = p(begin, end, value);
        ASSERT_FALSE(r);
        ASSERT_EQ(0, begin - input.begin());
    });
}

namespace top {

using namespace aquarius;


struct S : AQUARIUS_RHS(void, ch(' ', '\t', '\n', '\r'));

AQUARIUS_DEFINE_RULE(void, SPACE, *nonTerm<S>());

}

TEST(base, nterm) {
    using namespace top;
    using namespace aquarius;

    std::string line("  \t \n ");
    auto begin = line.begin();
    auto end = line.end();

    bool r = SPACE::pattern()(begin, end);
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(r));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(6, begin - line.begin()));

    auto rr = Parser<SPACE>()(line.begin(), line.end());
    ASSERT_NO_FATAL_FAILURE(ASSERT_TRUE(static_cast<bool>(rr)));
    ASSERT_NO_FATAL_FAILURE(ASSERT_FALSE(rr.hasResult()));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}