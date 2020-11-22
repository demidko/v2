cmake_minimum_required(VERSION 3.17)
#[[Без C++20 у нас не будет <bit>]]
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#[[Собираем основную утилиту]]
project(v2)
#[[Добавляем в сборку исходники]]
file(GLOB sources src/*.cpp)
add_executable(v2 ${sources})
#[[Указываем компилятору путь поиска своих h файлов]]
include_directories(src)
#[[Берем у vcpkg зависимость для парсинга аргументов командной строки]]
find_package(CLI11 CONFIG REQUIRED)
target_link_libraries(v2 PRIVATE CLI11::CLI11)

#[[Создаем тестовый проект]]
project(test)
#[[Подменяем для тестов точку входа]]
list(FILTER sources EXCLUDE REGEX ".*Main.cpp$")
#[[Добавляем исходники тестов]]
file(GLOB testSources test/*.cpp)
add_executable(test ${sources} ${testSources})
#[[Берём у vcpkg библиотеку для тестирования]]
find_package(Catch2 CONFIG REQUIRED)
target_link_libraries(test PRIVATE Catch2::Catch2)



