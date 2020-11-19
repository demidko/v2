cmake_minimum_required(VERSION 3.17)
project(v2)
set(CMAKE_CXX_STANDARD 20)
find_package(CLI11 CONFIG REQUIRED)
add_executable(
    v2
    src/Main.cpp
    src/Parser.cpp
    src/Compressor.cpp
    src/Decompressor.cpp
)
target_link_libraries(v2 PRIVATE CLI11::CLI11)


