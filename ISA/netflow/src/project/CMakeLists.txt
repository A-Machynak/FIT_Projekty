cmake_minimum_required(VERSION 3.2)
project(NetflowLib C CXX)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCE_FILES 
	*.cpp
	*.c++
)

file(GLOB HEADER_FILES
	*.h
	*.hpp
)

add_library(NetflowLib SHARED STATIC ${SOURCE_FILES} ${HEADER_FILES})
