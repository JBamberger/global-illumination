
set(SOURCES
	"include/gui.h"
	"include/viewer.h"
	"src/main.cpp"
)

add_executable(global_illu ${SOURCES})
target_include_directories(global_illu PRIVATE "include")
target_link_libraries(global_illu PRIVATE rt_lib ${CMAKE_THREAD_LIBS_INIT})