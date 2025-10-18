cmake_minimum_required(VERSION 3.22.1)
project(lw10)

set(CMAKE_CXX_STANDARD 20)

# Установка выходной папки для DLL и EXE
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

# Добавление поддиректории src
add_subdirectory(src)