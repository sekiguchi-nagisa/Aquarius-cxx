#+++++++++++++++++++++#
#     build gtest     #
#+++++++++++++++++++++#

if (CMAKE_VERSION VERSION_LESS 3.2)
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
else ()
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
endif ()

download_project(
        PROJ googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.11.0
        GIT_PROGRESS 1
        ${UPDATE_DISCONNECTED_IF_AVAILABLE}
)

add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
include_directories("${gtest_SOURCE_DIR}/include" "${gmock_SOURCE_DIR}/include")

#++++++++++++++++++++++++++++++++++++#
#     setup test in subdirectory     #
#++++++++++++++++++++++++++++++++++++#

add_subdirectory(base)
add_subdirectory(tuple)
add_subdirectory(type)
add_subdirectory(ascii)
