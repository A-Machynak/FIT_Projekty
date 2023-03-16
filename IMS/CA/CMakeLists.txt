cmake_minimum_required(VERSION 3.2)

project(cmake_project)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)

# Vypnuti buildu pro dokumentaci, testy a examples
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(deps/glfw-3.3.8)
add_subdirectory(deps/glad)
add_subdirectory(deps/stb)
