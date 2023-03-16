cmake_minimum_required(VERSION 3.2)
project(CA_Model C CXX)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCE_FILES
	*.cpp
	*.c++
)

file(GLOB_RECURSE HEADER_FILES
	*.h
	*.hpp
)

add_library(CA_Model_Lib SHARED STATIC ${SOURCE_FILES} ${HEADER_FILES})
