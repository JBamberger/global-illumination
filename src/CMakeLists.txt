find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Threads)

set(SOURCES main.cpp)
set(HEADERS
	include/gui.h
	include/image.h
	include/ray.h
	include/entities.h
	include/camera.h
	include/raytracer.h
	include/viewer.h
	include/octree.h
	include/bbox.h
	include/material.h
)



add_library(global_illu_lib
	ExplicitEntity.cpp
	ImplicitSphere.cpp
	Triangle.cpp
)
target_include_directories(global_illu_lib PUBLIC
	"${CMAKE_SOURCE_DIR}/include"
	"${CMAKE_SOURCE_DIR}/3rd_party"
)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(global_illu_lib PUBLIC OpenMP::OpenMP_CXX)
endif()


add_executable(global_illu ${SOURCES})
target_include_directories(global_illu PRIVATE
	"${CMAKE_SOURCE_DIR}/include"
)
target_link_libraries(global_illu PRIVATE
	global_illu_lib
	Qt5::Widgets
	${CMAKE_THREAD_LIBS_INIT}
)

option(CLANG_FORMAT_TARGET "automatic clang format" OFF)
if(CLANG_FORMAT_TARGET)
    include(ClangFormat)
    add_clang_format(TARGET global_illu STYLE "LLVM" FILES ${SOURCES} ${HEADERS})
endif(CLANG_FORMAT_TARGET)
