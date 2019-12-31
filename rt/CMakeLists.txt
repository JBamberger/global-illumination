
set(SOURCES
	"include/camera.h"
	"include/Material.h"
	"include/Entity.h"
	"include/image.h"
	"include/Ray.h"
    "include/CheckerboardMaterial.h" "src/CheckerboardMaterial.cpp"
    "include/SimpleMaterial.h"       "src/SimpleMaterial.cpp"
	"include/ExplicitEntity.h"       "src/ExplicitEntity.cpp"
	"include/ImplicitSphere.h"       "src/ImplicitSphere.cpp"
	"include/Triangle.h"             "src/Triangle.cpp"
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