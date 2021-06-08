cmake_minimum_required(VERSION 2.7)

project(Aquarius-cxx)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules")
include(DownloadProject)


#+++++++++++++++++++++++++++++++++#
#     set coverage build type     #
#+++++++++++++++++++++++++++++++++#

set(SANITIZER "" CACHE STRING "which sanitizer to use")

if(NOT ("${SANITIZER}" STREQUAL ""))
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=${SANITIZER} -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=${SANITIZER} -fno-omit-frame-pointer")
endif()

set(CMAKE_C_FLAGS_COVERAGE "-g -fprofile-arcs -ftest-coverage")
set(CMAKE_CXX_FLAGS_COVERAGE "-g -fprofile-arcs -ftest-coverage")

#+++++++++++++++++++++++++++++#
#     set compiler option     #
#+++++++++++++++++++++++++++++#

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic-errors -fno-exceptions -fno-rtti")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-maybe-uninitialized")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-warning-option")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")


include_directories(include)

add_subdirectory(example/json)
add_subdirectory(example/json2)
add_subdirectory(example/json3)

enable_testing()
add_subdirectory(test)

#+++++++++++++++++++++++++#
#     show enviroment     #
#+++++++++++++++++++++++++#

message("")
message("+++++ List of System Configuration +++++")
message(STATUS "CMAKE_BUILD_TYPE                 = ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_SYSTEM                     = ${CMAKE_SYSTEM}")
message(STATUS "CMAKE_CXX_COMPILER_ID            = ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER_VERSION       = ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "CMAKE_CXX_COMPILER               = ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_CXX_FLAGS                  = ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG            = ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMAKE_CXX_FLAGS_RELEASE          = ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "CMAKE_CXX_FLAGS_RELWITHDEBINFO   = ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
message(STATUS "CMAKE_INSTALL_PREFIX             = ${CMAKE_INSTALL_PREFIX}")
message("")
