cmake_minimum_required(VERSION 3.2)

project(cmake_project)

set(CMAKE_CXX_STANDARD 17)

set(INCLUDE_DIRS src/project)

#add_subdirectory(test)
add_subdirectory(src)
