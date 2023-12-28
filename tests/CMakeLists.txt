cmake_minimum_required(VERSION 3.23)
project(notoufy)
enable_testing()
include(GoogleTest)
set(CMAKE_CXX_STANDARD 23)

include(FetchContent)
FetchContent_Declare(gtest
        GIT_REPOSITORY      https://github.com/google/googletest
        GIT_TAG             v1.14.0
)
FetchContent_MakeAvailable(gtest)

add_executable(test_notoufy test_notoufy.cpp)

target_include_directories(
        test_notoufy
        PRIVATE
        ../include
)

target_link_libraries(
        test_notoufy
        PRIVATE
        gtest
        gtest_main
)

add_test(
        NAME test_notoufy
        COMMAND test_notoufy --gtest_output=xml:test_notoufy.xml
)