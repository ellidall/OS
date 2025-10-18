# Создание динамической библиотеки
add_library(QuadraticSolver SHARED quadraticSolver.cpp)

# Макрос для компиляции DLL
target_compile_definitions(QuadraticSolver PRIVATE COMPILING_DLL)

# Установка папки для сборки DLL
set_target_properties(QuadraticSolver PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        OUTPUT_NAME "QuadraticSolverWin"
)