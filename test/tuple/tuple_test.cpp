#include <gtest/gtest.h>

#include <internal/tuples.hpp>

using namespace aquarius;

TEST(tuple, case1) {
    std::tuple<int, std::string> t;

    static_assert(misc::isTuple<decltype(t)>::value, "must be tuple");

    auto t2 = std::make_tuple(45, t, 34.9);

    static_assert(misc::isTuple<decltype(t2)>::value, "must be tuple");
    static_assert(!misc::isTuple<decltype(std::get<0>(t2))>::value, "must not be tuple");
    static_assert(misc::isTuple<decltype(std::get<1>(t2))>::value, "must be tuple");
    static_assert(!misc::isTuple<decltype(std::get<2>(t2))>::value, "must not be tuple");
}

TEST(tuple, case2) {
    auto v1 = misc::appendToTuple(1, "hello");
    static_assert(misc::isTuple<decltype(v1)>::value, "must be tuple");
    static_assert(std::is_same<decltype(std::make_tuple(1, "hello")), decltype(v1)>::value, "must be same type");

    auto v2 = misc::appendToTuple(true, std::move(v1));
    static_assert(misc::isTuple<decltype(v2)>::value, "must be tuple");
    static_assert(std::is_same<decltype(std::make_tuple(true, 1, "hello")), decltype(v2)>::value, "must be same type");

    auto v3 = misc::appendToTuple(std::move(v2), 34.9);
    static_assert(misc::isTuple<decltype(v3)>::value, "must be tuple");
    static_assert(std::is_same<decltype(std::make_tuple(true, 1, "hello", 3.6)), decltype(v3)>::value, "must be same type");

    auto v4 = misc::appendToTuple(std::move(v3), std::make_tuple(4u, "hello"));
    static_assert(misc::isTuple<decltype(v4)>::value, "must be tuple");
    static_assert(std::is_same<decltype(std::make_tuple(true, 1, "hello", 3.6, 4u, "hello")),
            decltype(v4)>::value, "must be same type");
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

