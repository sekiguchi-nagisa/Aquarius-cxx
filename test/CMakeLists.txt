#+++++++++++++++++++++#
#     build gtest     #
#+++++++++++++++++++++#

set(GTEST_V "1.7.0")
set(GTEST_URL "https://github.com/google/googletest/archive/release-${GTEST_V}.zip")
set(GTEST_PATH ${CMAKE_CURRENT_SOURCE_DIR}/googletest-${GTEST_V}.zip)
set(GTEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/googletest-release-${GTEST_V})

if(NOT EXISTS "${GTEST_DIR}")
    file(DOWNLOAD "${GTEST_URL}" "${GTEST_PATH}" SHOW_PROGRESS)
    execute_process(COMMAND unzip ${GTEST_PATH} -d ${CMAKE_CURRENT_SOURCE_DIR})
endif()

add_subdirectory(${GTEST_DIR})


#++++++++++++++++++++++++++++++++++++#
#     setup test in subdirectory     #
#++++++++++++++++++++++++++++++++++++#

include_directories(${GTEST_DIR}/include)

add_subdirectory(base)
add_subdirectory(tuple)
add_subdirectory(type)
add_subdirectory(ascii)
