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
        "include/Gui.h"
        "include/Viewer.h"
        "src/main.cpp"
        "src/Viewer.cpp"
        "src/Gui.cpp")

add_executable(global_illu ${SOURCES})
target_include_directories(global_illu PRIVATE "include")
target_link_libraries(global_illu PRIVATE rt_lib ${CMAKE_THREAD_LIBS_INIT})
