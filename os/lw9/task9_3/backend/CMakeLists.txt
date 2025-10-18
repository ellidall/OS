cmake_minimum_required(VERSION 3.22)
project(Backend)

add_executable(backend main.cpp)

find_package(nlohmann_json 3.10.0 REQUIRED)
target_link_libraries(backend PRIVATE nlohmann_json::nlohmann_json)