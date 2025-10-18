# Создание динамической библиотеки
add_library(QuadraticSolverLin SHARED quadraticSolverLin.cpp)

# Макрос для компиляции DLL
target_compile_definitions(QuadraticSolverLin PRIVATE COMPILING_SO)

# Установка папки для сборки DLL
set_target_properties(QuadraticSolverLin PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        OUTPUT_NAME "QuadraticSolverLin"
)