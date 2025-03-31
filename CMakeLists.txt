cmake_minimum_required(VERSION 3.30)
project(derp LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 23)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW Library only" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "GLFW Library only" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "GLFW Library only" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "GLFW Library only" FORCE)

set(GLM_BUILD_TESTS OFF CACHE BOOL "GLM Library only" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Static GLM library" FORCE)

add_subdirectory(include/glfw)
add_subdirectory(include/glm)

add_executable(derp
    src/main.cpp
    src/glad.c
    include/derp/shader.cpp
    include/derp/shader.hpp
)

target_compile_definitions(derp PRIVATE
    RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources"
)

target_include_directories(derp PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(derp PRIVATE
    glfw
    glm::glm
)
