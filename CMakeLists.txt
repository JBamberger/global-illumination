cmake_minimum_required(VERSION 3.2)

project(global-illu)

enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

if (MSVC)
add_definitions(-DNOMINMAX)
endif()

find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Threads)
find_package(glm CONFIG REQUIRED)


add_subdirectory("rt")
add_subdirectory("app")


