# Создание динамической библиотеки
add_library(StudentDb SHARED studentDb.cpp)

# Макрос для компиляции
if (WIN32)
    target_compile_definitions(StudentDb PRIVATE COMPILING_DLL)
else()
    target_compile_definitions(StudentDb PRIVATE COMPILING_SO)
endif()

# Установка папки для сборки
set_target_properties(StudentDb PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        OUTPUT_NAME "StudentDb"
)