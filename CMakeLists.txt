cmake_minimum_required(VERSION 3.24)
project(OpenGL)

set(CMAKE_CXX_STANDARD 17)
file(GLOB sourceFiles ${CMAKE_SOURCE_DIR}/src/*.cpp external/ImGui/*.cpp external/ImGui/backends/imgui_impl_glfw.cpp external/ImGui/backends/imgui_impl_opengl3.cpp)
add_executable(OpenGL ${sourceFiles})
include_directories(external/glfw/include external/glm external/assimp/include)
target_include_directories(OpenGL PUBLIC include/ external/ImGui external/ImGui/backends)

add_library(assimp SHARED IMPORTED)
set_target_properties(assimp PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/external/assimp/bin/libassimp.so)

#Copy shaders folder to binary directory
add_custom_target(Shaders)
add_dependencies(OpenGL Shaders Textures)
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

file(COPY ${CMAKE_SOURCE_DIR}/models DESTINATION ${CMAKE_BINARY_DIR})

#Build GLFW
add_subdirectory(external/glfw)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glm)
add_subdirectory(external/glad)
add_subdirectory(external/reactphysics3d)
target_link_libraries(OpenGL glfw glm glad assimp reactphysics3d)