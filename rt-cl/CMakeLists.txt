
set(SOURCES
	"include/rt.h"	         "src/rt.cpp"
)

find_package(OpenCL REQUIRED)

add_library(rtcl_lib ${SOURCES})
target_include_directories(rtcl_lib PUBLIC "include")
target_link_libraries(rtcl_lib PUBLIC OpenCL::OpenCL glm)