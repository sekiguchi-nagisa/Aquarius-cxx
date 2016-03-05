#====================#
#     tuple test     #
#====================#

set(TEST_NAME tuple_test)
set(SOURCE_FILES tuple_test.cpp)

add_executable(${TEST_NAME} ${SOURCE_FILES})
target_link_libraries(${TEST_NAME} gtest gtest_main)
add_test(${TEST_NAME} ${TEST_NAME})