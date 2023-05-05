cmake_minimum_required(VERSION 3.23)
project(notoufy)

set(CMAKE_CXX_STANDARD 23)
set(DEPS deps/)
set(GTEST_SOURCE_DIR deps/src/libgtest)
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/${DEPS}/libs)

include(ExternalProject)

ExternalProject_Add(gtest
        PREFIX                      build/
        URL                         https://github.com/google/googletest/archive/refs/tags/v1.13.0.tar.gz
        SOURCE_DIR                  ${CMAKE_CURRENT_SOURCE_DIR}/${GTEST_SOURCE_DIR}
        BUILD_IN_SOURCE             true
        INSTALL_COMMAND             ""
        )

include_directories(
        ../include
        ${CMAKE_CURRENT_SOURCE_DIR}/${GTEST_SOURCE_DIR}/googletest/include/
)

add_executable(test_notoufy test_notoufy.cpp)
target_link_libraries(test_notoufy ${CMAKE_CURRENT_SOURCE_DIR}/${GTEST_SOURCE_DIR}/lib/libgtest.a)
