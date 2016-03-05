#include <gtest/gtest.h>

#include <internal/tuples.hpp>

using namespace aquarius;

TEST(tuple, isTuple) {
    std::tuple<int, std::string> t;

    static_assert(misc::isTuple<decltype(t)>::value, "must be tuple");

    auto t2 = std::make_tuple(45, t, 34.9);

    static_assert(misc::isTuple<decltype(t2)>::value, "must be tuple");
    static_assert(!misc::isTuple<decltype(std::get<0>(t2))>::value, "must not be tuple");
    static_assert(misc::isTuple<decltype(std::get<1>(t2))>::value, "must be tuple");
    static_assert(!misc::isTuple<decltype(std::get<2>(t2))>::value, "must not be tuple");
}

TEST(tuple, concatenation) {
    auto v1 = misc::catAsTuple(1, "hello");
    static_assert(misc::isTuple<decltype(v1)>::value, "must be tuple");
    static_assert(std::is_same<decltype(std::make_tuple(1, "hello")), decltype(v1)>::value, "must be same type");

    auto v2 = misc::catAsTuple(true, std::move(v1));
    static_assert(misc::isTuple<decltype(v2)>::value, "must be tuple");
    static_assert(std::is_same<decltype(std::make_tuple(true, 1, "hello")), decltype(v2)>::value, "must be same type");

    auto v3 = misc::catAsTuple(std::move(v2), 34.9);
    static_assert(misc::isTuple<decltype(v3)>::value, "must be tuple");
    static_assert(std::is_same<decltype(std::make_tuple(true, 1, "hello", 3.6)), decltype(v3)>::value, "must be same type");

    auto v4 = misc::catAsTuple(std::move(v3), std::make_tuple(4u, "hello"));
    static_assert(misc::isTuple<decltype(v4)>::value, "must be tuple");
    static_assert(std::is_same<decltype(std::make_tuple(true, 1, "hello", 3.6, 4u, "hello")),
            decltype(v4)>::value, "must be same type");
}

struct Negate {
    int operator()(int a) const {
        return -a;
    }
};

struct Sum {
    int operator()(int a, int b) const {
        return a + b;
    }
};

struct Cat {
    std::string operator()(std::string &&a, std::string &&b) const {
        return a + b;
    }
};

TEST(tuple, unpack1) {
    Negate negate;

    static_assert(std::is_same<int, decltype(misc::unpackAndApply(negate, 1))>::value, "must be int");
    auto r = misc::unpackAndApply(negate, 1);
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(-1, r));
}

TEST(tuple, unpack2) {
    Sum sum;

    static_assert(std::is_same<int, decltype(misc::unpackAndApply(sum, std::make_tuple(1, 2)))>::value, "must be int");
    auto r = misc::unpackAndApply(sum, std::make_tuple(1, 2));
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(3, r));

    Cat cat;
    auto v = misc::unpackAndApply(cat, std::make_tuple(std::string("hello "), std::string("world")));
    static_assert(std::is_same<std::string, decltype(v)>::value, "must be int");
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ("hello world", v));
}

struct Unwrap {
    template <typename T>
    T operator()(Optional<T> &&t) const {
        return std::move(t.get());
    }
};

TEST(tuple, unpack3) {
    Unwrap unwrap;

    auto r = misc::unpackAndApply(unwrap, std::make_tuple(Optional<int>(100)));
    static_assert(std::is_same<int, decltype(r)>::value, "must be int");
    ASSERT_NO_FATAL_FAILURE(ASSERT_EQ(100, r));
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

