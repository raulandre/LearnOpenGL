cmake_minimum_required(VERSION 3.24)
project(OpenGL)

set(CMAKE_CXX_STANDARD 17)
add_executable(OpenGL main.cpp external/glad/src/glad.c)
find_package(OpenGL REQUIRED)

add_subdirectory(external/glfw)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
target_link_libraries(OpenGL glfw)

include_directories(external/glfw/include external/glad/include)