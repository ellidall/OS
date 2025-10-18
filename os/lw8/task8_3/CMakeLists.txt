add_executable(task8_3 main.cpp)

find_package(wxWidgets REQUIRED)
include(${wxWidgets_USE_FILE})
target_link_libraries(task8_3 ${wxWidgets_LIBRARIES})