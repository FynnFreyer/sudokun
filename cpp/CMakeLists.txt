cmake_minimum_required(VERSION 3.5)

project(sudokun LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} src/utils.cpp src/sudoku.cpp src/main.cpp)
