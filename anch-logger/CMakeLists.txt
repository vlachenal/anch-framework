cmake_minimum_required(VERSION 2.6)

# Project anch logger
# ANother C++ Hack 
project(anch-logger)
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

# Add GCC definitions
add_definitions("-Wall -Werror")

# Include librairies
include(FindThreads)
find_package(Threads)

# Includes location
include_directories(inc)

# Retrieve every files in project
file(
  GLOB_RECURSE
  source_files
  src/*
  inc/*
)

file(
  GLOB_RECURSE
  test_files
  inc/*
  test/*
)

# Declare exec
add_library(
  anch-logger
  SHARED
  ${source_files}
)

add_executable(
  anch-logger-test
  ${test_files}
)

# Link with libraries
target_link_libraries(
  anch-logger
  ${CMAKE_THREAD_LIBS_INIT}
)

target_link_libraries(
  anch-logger-test
  anch-logger
)
