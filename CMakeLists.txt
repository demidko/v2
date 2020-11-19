cmake_minimum_required(VERSION 3.17)
project(v2)
set(CMAKE_CXX_STANDARD 20)
find_package(CLI11 CONFIG REQUIRED)
file(GLOB sources src/*.cpp)
add_executable(v2 ${sources})
target_link_libraries(v2 PRIVATE CLI11::CLI11)


