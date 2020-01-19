
set(SOURCES
	"include/camera.h"
	"include/Texture.h"
	"include/Material.h"
	"include/image.h"
	"include/Ray.h"
	"include/NDChecker.h"
	"include/Entity.h"				 "src/Entity.cpp"
	"include/ExplicitEntity.h"       "src/ExplicitEntity.cpp"
    "include/RayTracer.h"            "src/RayTracer.cpp"
    "include/BoundingBox.h"          "src/BoundingBox.cpp"
    "include/Octree.h"               "src/Octree.cpp"
    "include/entities.h"             "src/entities.cpp"
)

add_library(rt_lib ${SOURCES})
target_include_directories(rt_lib PUBLIC "include")
target_link_libraries(rt_lib PUBLIC Qt5::Widgets glm)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(rt_lib PUBLIC OpenMP::OpenMP_CXX)
endif()

add_subdirectory(test)