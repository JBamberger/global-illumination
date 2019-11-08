cmake_minimum_required(VERSION 3.2)

project(global-illu)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 14)

if (MSVC)
add_definitions(-DNOMINMAX)
endif()


add_subdirectory(src)

enable_testing()
add_subdirectory(test)