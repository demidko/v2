cmake_minimum_required(VERSION 3.17)
project(v2)

#[[без C++20 у нас не будет легкой поддержки концептов и оптимизированных "из коробки" побитовых операций]]
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#[[берем у vcpkg зависимость для парсинга аргументов командной строки]]
find_package(CLI11 CONFIG REQUIRED)

file(GLOB src src/*.cpp)
add_executable(v2 ${src})
target_link_libraries(v2 PRIVATE CLI11::CLI11)