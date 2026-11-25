cmake_minimum_required(VERSION 3.16)

project(FileComparator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(FileComparator main.cpp filecomparator.cpp filecomparator.h)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/input.txt ${CMAKE_CURRENT_BINARY_DIR}/input.txt COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/file1.txt ${CMAKE_CURRENT_BINARY_DIR}/file1.txt COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/file2.txt ${CMAKE_CURRENT_BINARY_DIR}/file2.txt COPYONLY)
