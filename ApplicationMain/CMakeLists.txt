cmake_minimum_required(VERSION 3.23)
project(AppMain)

find_package(Boost REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(Boost_USE_STATIC_LIBS ON)

add_executable(AppMain main.cpp DataRow.cpp ../include/DataRow.h DataColumn.cpp ../include/DataColumn.h CSV_File.cpp ../include/CSV_File.h)

target_include_directories(AppMain PUBLIC ${CMAKE_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS})

add_compile_options(-Ofast -v)
set(CMAKE_EXE_LINKER_FLAGS " -static")

if (WIN32)
    target_link_libraries(AppMain ${Boost_LIBRARIES} ws2_32 wsock32 -static-libgcc -static-libstdc++)
endif(WIN32)

if (UNIX)
    target_link_libraries(AppMain ${Boost_LIBRARIES} -static-libgcc -static-libstdc++)
    # there might be some more platform specific networking libraries for mac and linux
endif(UNIX)
