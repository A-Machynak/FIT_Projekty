cmake_minimum_required(VERSION 3.0)
project(Glad)

set(SOURCE_FILES
	src/glad.c
)

set(HEADER_FILES
	include/glad/glad.h
	include/KHR/khrplatform.h
)

add_library(glad ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(glad PUBLIC include/)
