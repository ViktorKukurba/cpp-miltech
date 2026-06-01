cmake_minimum_required(VERSION 3.20)

# Окремий CMake-проєкт для домашньої роботи 04. Його можна збирати як через root CMakeLists.txt, так
# і напряму з цієї директорії.
project(ballistics CXX)

# Код домашньої роботи збирається з тим самим стандартом, що й лекційні приклади.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(ballistics_lib src/ballistics.cpp)
target_include_directories(ballistics_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(ballistics src/main.cpp)
target_link_libraries(ballistics PRIVATE ballistics_lib)
add_executable(ballistics_tests tests/ballistics_tests.cpp)

# Базові compiler warnings допомагають побачити підозрілі місця до runtime.
target_compile_options(ballistics PRIVATE -Wall -Wextra -pedantic)

target_link_libraries(ballistics_tests PRIVATE GTest::gtest GTest::gtest_main ballistics_lib)
include(GoogleTest)
gtest_discover_tests(ballistics_tests WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
