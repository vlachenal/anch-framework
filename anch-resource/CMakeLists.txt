cmake_minimum_required(VERSION 2.6)

# Project anch resource
# ANother C++ Harsh/Huge/Hack 
project(anch-resource)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})

# Activate C++11
add_definitions("-std=c++11")
add_definitions("-Wall")

# Include librairies
#include(FindThreads)
#find_package(Threads)

# Includes location
include_directories(inc)

# Retrieve every files in project
file(
	GLOB_RECURSE
	source_files
	src/*
	inc/*
)

# Declare exec
add_library(
	anch-resource
	${source_files}
)

# Link with libraries
#target_link_libraries(
#	anch-resource
#	${CMAKE_THREAD_LIBS_INIT}
#)
