cmake_minimum_required(VERSION 3.22.1)

project(task9_2)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

add_executable(task9_2 main.cpp)

target_link_libraries(task9_2 Qt5::Widgets)