cmake_minimum_required(VERSION 3.23)
project(AppMain)

set(CMAKE_CXX_STANDARD 14)

add_executable(AppMain main.cpp ../k-d_tree/k-d_tree.h ../tests/k-d_tree_tests/k-d_tree_tests.cpp)