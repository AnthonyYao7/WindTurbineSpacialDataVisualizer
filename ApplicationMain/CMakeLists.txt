cmake_minimum_required(VERSION 3.23)
project(AppMain)

find_package(Boost REQUIRED)

set(CMAKE_CXX_STANDARD 20)

add_executable(AppMain main.cpp DataRow.cpp ../include/DataRow.h DataColumn.cpp ../include/DataColumn.h CSV_File.cpp ../include/CSV_File.h)

target_include_directories(AppMain PUBLIC ${CMAKE_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS})

add_compile_options(-g -v)


if (WIN32)
    target_link_libraries(AppMain ${Boost_LIBRARIES} ws2_32 wsock32)
endif(WIN32)

if (UNIX)
    target_link_libraries(AppMain ${Boost_LIBRARIES})
    # there might be some more platform specific networking libraries for mac and linux
endif(UNIX)
