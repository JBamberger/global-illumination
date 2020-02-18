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


find_package(GTest CONFIG REQUIRED)

add_executable(unit_tests
    main.cpp
    entities-test.cpp
    bbox-test.cpp
    checkerboard-test.cpp
    uv-mapping-test.cpp
)

target_link_libraries(
	unit_tests
	PRIVATE GTest::gtest GTest::gtest_main
	rt_lib
)

add_test(NAME unit COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/unit_tests)
