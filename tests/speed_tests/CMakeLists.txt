cmake_minimum_required(VERSION 3.14)
project(speed_tests)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
        speedtest
        ../speed_tests/speed_test.cpp ../speed_tests/speed_test.h ../speed_tests/speed_test.cpp)

target_include_directories(
        kdtreetest PUBLIC ${CMAKE_SOURCE_DIR}/include
)