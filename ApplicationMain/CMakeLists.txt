cmake_minimum_required(VERSION 3.23)
project(AppMain)

set(CMAKE_CXX_STANDARD 14)

add_executable(AppMain main.cpp)

target_include_directories(AppMain PUBLIC ${CMAKE_SOURCE_DIR}/include)
