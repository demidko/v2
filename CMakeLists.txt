cmake_minimum_required(VERSION 3.17)
#[[Без C++20 у нас не будет <bit>]]
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(v2)
#[[Берем у vcpkg зависимость для парсинга аргументов командной строки]]
find_package(CLI11 CONFIG REQUIRED)
#[[Добавляем в сборку исходники и зависимости]]
file(GLOB src src/*.cpp)
add_executable(v2 ${src})
target_link_libraries(v2 PRIVATE CLI11::CLI11)

#[[Создаем тестовый проект]]
project(v2test)
#[[Берём у vcpkg библиотеку для тестирования]]
find_package(Catch2 CONFIG REQUIRED)
#[[Подменяем для тестов точку входа]]
list(FILTER src EXCLUDE REGEX ".*Main.cpp$")
file(GLOB test test/*.cpp)
add_executable(v2test ${src} ${test})
target_link_libraries(v2test PRIVATE Catch2::Catch2)



