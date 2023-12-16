cmake_minimum_required(VERSION 3.25)
project(voyager)

set(CMAKE_CXX_STANDARD 23)

add_executable(voyager main.cc
        util.h
        gemini.h
        gemini.cc
)

find_package(OpenSSL REQUIRED)
target_link_libraries(voyager OpenSSL::SSL)
