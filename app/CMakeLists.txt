
set(SOURCES
        "include/Gui.h"
        "include/Viewer.h"
        "src/main.cpp"
        "src/Viewer.cpp"
        "src/Gui.cpp")

add_executable(global_illu ${SOURCES})
target_include_directories(global_illu PRIVATE "include")
target_link_libraries(global_illu PRIVATE rt_lib ${CMAKE_THREAD_LIBS_INIT})