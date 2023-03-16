cmake_minimum_required(VERSION 3.2)
project(CA_Visualization C CXX)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCE_FILES
	*.cpp
	*.c++
)

file(GLOB_RECURSE HEADER_FILES
	*.h
	*.hpp
)

include_directories(../model)

add_executable(CA_Visualization ${SOURCE_FILES} ${HEADER_FILES})

# Opengl knihovna
find_package(OpenGL REQUIRED)

target_link_libraries(CA_Visualization stb glfw glad OpenGL::GL CA_Model_Lib)
