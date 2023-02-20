cmake_minimum_required(VERSION 3.24)
project(OpenGL)

set(CMAKE_CXX_STANDARD 17)
add_executable(OpenGL src/main.cpp external/glad/src/glad.c src/shader.cpp)
include_directories(external/glfw/include external/glad/include include/ external/glm)

#Copy shaders folder to binary directory
add_custom_target(Shaders)
add_dependencies(OpenGL Shaders)
file(GLOB shaderFiles ${CMAKE_SOURCE_DIR}/shaders/*.glsl)

add_custom_command(TARGET Shaders PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/shaders)
add_custom_command(TARGET Shaders PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/shaders)

foreach(shaderFile ${shaderFiles})
    add_custom_command(TARGET Shaders POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${shaderFile} ${CMAKE_BINARY_DIR}/shaders)
endforeach()

add_custom_target(Textures)
add_dependencies(OpenGL Textures)
file(GLOB textureFiles ${CMAKE_SOURCE_DIR}/textures/*.jpg ${CMAKE_SOURCE_DIR}/textures/*.png)

add_custom_command(TARGET Textures PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/textures)
add_custom_command(TARGET Textures PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/textures)

foreach(textureFile ${textureFiles})
    add_custom_command(TARGET Textures POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${textureFile} ${CMAKE_BINARY_DIR}/textures)
endforeach()

#Build GLFW
add_subdirectory(external/glfw)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glm)

target_link_libraries(OpenGL glfw glm)