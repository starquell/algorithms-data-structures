cmake_minimum_required(VERSION 3.9)

project(tests)
set(CMAKE_CXX_STANDARD 17)

add_executable(tests tests.cpp)

add_test(NAME Tests COMMAND tests)
