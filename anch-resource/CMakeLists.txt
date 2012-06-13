cmake_minimum_required(VERSION 2.6)

# Project anch resource
# ANother C++ Harsh/Huge/Hack 
project(anch-resource)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})

# Check GCC version +
execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                OUTPUT_VARIABLE GCC_VERSION)
if (GCC_VERSION VERSION_LESS 4.5)
  message(FATAL_ERROR "Version < 4.5. This library is based on C++11 standard. Exit.")
elseif(GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7)
  message(STATUS "Version >= 4.7 use -std=c++11")
  add_definitions("-std=c++11")
else()
  message(STATUS "4.5 <= Version < 4.7 use -std=c++0x")
  add_definitions("-std=c++0x")
endif()
# Check GCC version -

# Activate C++11
add_definitions("-Wall")

# Include librairies
include(FindThreads)
find_package(Threads)

find_package(Boost COMPONENTS regex)

# Includes location
include_directories(
  inc
  ${Boost_INCLUDE_DIRS}
)

link_directories(
  ${Boost_LIBRARY_DIRS}
)

# Retrieve every files in project
file(
  GLOB_RECURSE
  source_files
  src/*
  inc/*
)

# Retrieve every files in project
file(
  GLOB_RECURSE
  test_files
  inc/*
  test/*
)

# Declare exec
add_library(
  anch-resource
  SHARED
  ${source_files}
)

add_executable(
  anch-resource-test
  ${test_files}
)

# Link with libraries
target_link_libraries(
  anch-resource
  ${CMAKE_THREAD_LIBS_INIT}
  ${Boost_LIBRARIES}
)

target_link_libraries(
  anch-resource-test
  anch-resource
)
