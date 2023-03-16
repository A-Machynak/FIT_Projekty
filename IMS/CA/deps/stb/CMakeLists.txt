cmake_minimum_required(VERSION 3.2)
project(stb C CXX)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCE_FILES
	*.cpp
	*.c++
)

file(GLOB_RECURSE HEADER_FILES
	*.h
	*.hpp
)


add_library(stb SHARED STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(stb PUBLIC .)
set_target_properties(stb PROPERTIES LINKER_LANGUAGE CXX)
