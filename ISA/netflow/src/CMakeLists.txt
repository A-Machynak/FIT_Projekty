cmake_minimum_required(VERSION 3.2)

project(flow)

add_subdirectory(project)
set(SOURCE_FILES main.cpp)

include(pcap_cmake/FindPCAP.cmake)

add_executable(flow ${SOURCE_FILES})
target_link_libraries(flow NetflowLib ${PCAP_LIBRARY})
