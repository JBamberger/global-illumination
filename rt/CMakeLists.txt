#
#    Copyright 2020 Jannik Bamberger
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#


set(SOURCES
        "include/Camera.h"
        "include/Texture.h"
        "include/Image.h"
        "include/Ray.h"
        "include/NDChecker.h"
        "include/RandomUtils.h"
        "include/BVH.h" "src/BVH.cpp"
        "include/Material.h" "src/Material.cpp"
        "include/Entity.h" "src/Entity.cpp"
        "include/ExplicitEntity.h" "src/ExplicitEntity.cpp"
        "include/PathTracer.h" "src/PathTracer.cpp"
        "include/BoundingBox.h" "src/BoundingBox.cpp"
        "include/Octree.h" "src/Octree.cpp"
        "include/entities.h" "src/entities.cpp"
        "include/ObjReader.h" "src/ObjReader.cpp"
        include/Scene.h src/Scene.cpp src/Camera.cpp src/Image.cpp)

add_library(rt_lib ${SOURCES})
target_include_directories(rt_lib PUBLIC "include")
target_link_libraries(rt_lib PUBLIC Qt5::Widgets glm)

find_package(OpenMP)
if (OpenMP_CXX_FOUND)
    target_link_libraries(rt_lib PUBLIC OpenMP::OpenMP_CXX)
endif ()

add_subdirectory(test)
