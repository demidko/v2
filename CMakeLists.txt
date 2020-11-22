cmake_minimum_required(VERSION 3.17)
project(v2)
#[[Без C++20 у нас не будет <bit>]]
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#[[Берем у vcpkg зависимость для парсинга аргументов командной строки]]
find_package(CLI11 CONFIG REQUIRED)
#[[Добавляем в сборку исходники и зависимости]]
file(GLOB src src/*.cpp)
add_executable(v2 ${src})
target_link_libraries(v2 PRIVATE CLI11::CLI11)

#[[Создаем тестовый проект]]
project(v2test)
#[[Подменяем для тестов точку входа]]
list(FILTER src EXCLUDE REGEX ".*Main.cpp$")
#[[Добавляем в тестовый проект все исходники основного + исходники тестов]]
file(GLOB test test/*.cpp)
add_executable(v2test ${src} ${test})
#[[Добавляем зависимости для тестирования]]
find_package(GTest CONFIG REQUIRED)
target_link_libraries(v2test PRIVATE GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main)

