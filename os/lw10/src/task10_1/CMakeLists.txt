add_executable(task10_1 main.cpp)

# Связывание с DLL
target_link_libraries(task10_1 PRIVATE QuadraticSolver)

# Установка папки для сборки EXE
set_target_properties(task10_1 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)