#====================#
#     ascii_test     #
#====================#

set(TEST_NAME ascii_test)
set(SOURCE_FILES ascii_test.cpp)

add_executable(${TEST_NAME} ${SOURCE_FILES})
target_link_libraries(${TEST_NAME} gtest gtest_main)
add_test(${TEST_NAME} ${TEST_NAME})