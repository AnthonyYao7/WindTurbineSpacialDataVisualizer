cmake_minimum_required(VERSION 3.23)
project(AppMain)

find_package(Boost REQUIRED)

set(CMAKE_CXX_STANDARD 20)

add_executable(AppMain main.cpp)

target_include_directories(AppMain PUBLIC ${CMAKE_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS})

if (WIN32)
    target_link_libraries(AppMain ${Boost_LIBRARIES} ws2_32 wsock32)
endif(WIN32)

if (UNIX)
    target_link_libraries(AppMain ${Boost_LIBRARIES})
    # there might be some more platform specific networking libraries for mac and linux
endif(UNIX)
