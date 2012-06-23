cmake_minimum_required(VERSION 2.6)

# Project anch events
# ANother C++ Harsh/Huge/Hack 
project(anch-events)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include(AnchEvents.cmake.in)

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
include_directories(
  inc
)

## Retrieve every files in project
#file(
#  GLOB_RECURSE
#  source_files
#  src/*
#  inc/*
#)
#
## Declare exec
#add_library(
#  anch-events
#  SHARED
#  ${source_files}
#)
#
## Link with libraries
#target_link_libraries(
#  anch-events
#  ${CMAKE_THREAD_LIBS_INIT}
#)

if(ANCH_EVENTS_UNITARY_TEST)

  # Build parser unitary test +
  file(
    GLOB_RECURSE
    test_files
    inc/*
    test/testEvents.cpp
  )

  add_executable(
    anch-events-test
    ${test_files}
  )

  target_link_libraries(
    anch-events-test
    ${CMAKE_THREAD_LIBS_INIT}
    #anch-events
  )
  # Build parser unitary test -

  enable_testing()
  add_test(anch-events-test ${EXECUTABLE_OUTPUT_PATH}/anch-events-test main)

endif(ANCH_EVENTS_UNITARY_TEST)
