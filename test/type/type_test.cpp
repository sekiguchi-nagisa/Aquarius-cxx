#include <gtest/gtest.h>

#include <internal/misc.hpp>

using namespace aquarius;

template <typename T>
struct Holder {

};

template <typename T> struct Holder2;
using Holder3 = Holder2<int>;

TEST(type, case1) {
    static_assert(std::is_same<int, misc::param_type_of_t<Holder<int>>>::value, "must be same type");
    static_assert(std::is_same<int, misc::param_type_of_t<Holder2<int>>>::value, "must be same type");
    static_assert(std::is_same<int, misc::param_type_of_t<Holder3>>::value, "must be same type");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

