add_executable(task10_3 main.cpp)

# Связывание с SO
target_link_libraries(task10_3 PRIVATE StudentDb)

# Установка папки для сборки исполняемого файла
set_target_properties(task10_3 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
