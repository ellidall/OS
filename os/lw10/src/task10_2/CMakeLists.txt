add_executable(task10_2 main.cpp)

# Связывание с SO
target_link_libraries(task10_2 PRIVATE QuadraticSolverLin)

# Установка папки для сборки исполняемого файла
set_target_properties(task10_2 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)