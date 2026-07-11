cmake_minimum_required(VERSION 3.20)

# Окремий CMake-проєкт для домашньої роботи 04. Його можна збирати як через root CMakeLists.txt, так
# і напряму з цієї директорії.
project(mission_processor CXX)

# Код домашньої роботи збирається з тим самим стандартом, що й лекційні приклади.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(mission_processor_lib src/MissionProcessor.cpp)
target_include_directories(mission_processor_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(mission_processor src/main.cpp)
target_link_libraries(mission_processor PRIVATE mission_processor_lib)
# add_executable(mission_processor_tests tests/mission_processor_tests.cpp)

# Базові compiler warnings допомагають побачити підозрілі місця до runtime.
target_compile_options(mission_processor PRIVATE -Wall -Wextra -pedantic)

# target_link_libraries(mission_processor_tests PRIVATE GTest::gtest GTest::gtest_main
# mission_processor_lib) include(GoogleTest) gtest_discover_tests(mission_processor_tests
# WORKING_DIRECTORY
# "${CMAKE_CURRENT_SOURCE_DIR}")
