# Указываем имя исполняемого файла и его исходные файлы
add_executable(task7_3 main.cpp)

# Проверяем наличие библиотеки liburing
find_library(LIBURING_LIBRARY uring REQUIRED)
if(NOT LIBURING_LIBRARY)
    message(FATAL_ERROR "liburing library not found!")
endif()

# Подключаем библиотеку к проекту
target_link_libraries(task7_3 PRIVATE ${LIBURING_LIBRARY})