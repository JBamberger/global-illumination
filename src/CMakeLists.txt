find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Threads)

set(SOURCES
	main.cpp
)
set(HEADERS
	include/bbox.h
	include/camera.h
    include/checkerboard_material.h
	include/entities.h
	include/entity.h
	include/explicit_entity.h
	include/gui.h
	include/image.h
	include/implicit_sphere.h
	include/material.h
	include/octree.h
	include/ray.h
	include/raytracer.h
    include/simple_material.h
	include/triangle.h
	include/viewer.h
)



add_library(global_illu_lib
    checkerboard_material.cpp
    simple_material.cpp
	explicit_entity.cpp
	implicit_sphere.cpp
	triangle.cpp
    raytracer.cpp
)
target_include_directories(global_illu_lib PUBLIC
	"${CMAKE_SOURCE_DIR}/include"
	"${CMAKE_SOURCE_DIR}/3rd_party"
)
target_link_libraries(global_illu_lib PUBLIC
    Qt5::Widgets
)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(global_illu_lib PUBLIC OpenMP::OpenMP_CXX)
endif()


add_executable(global_illu
	main.cpp
)
target_include_directories(global_illu PRIVATE
	"${CMAKE_SOURCE_DIR}/include"
)
target_link_libraries(global_illu PRIVATE
	global_illu_lib
	Qt5::Widgets
	${CMAKE_THREAD_LIBS_INIT}
)

# option(CLANG_FORMAT_TARGET "automatic clang format" OFF)
# if(CLANG_FORMAT_TARGET)
#     include(ClangFormat)
#     add_clang_format(TARGET global_illu STYLE "LLVM" FILES ${SOURCES} ${HEADERS})
# endif(CLANG_FORMAT_TARGET)
