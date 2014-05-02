cmake_minimum_required(VERSION 2.6)

# Project anch resource
# ANother C++ Harsh/Huge/Hack
project(anch-resource)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include(AnchResource.cmake.in)

# Check GCC version +
execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                OUTPUT_VARIABLE GCC_VERSION)
if (GCC_VERSION VERSION_LESS 4.5)
  message(FATAL_ERROR "Version < 4.5. This library is based on C++11 standard. Exit.")
elseif(GCC_VERSION VERSION_GREATER 4.9 OR GCC_VERSION VERSION_EQUAL 4.9)
  message(STATUS "Version >= 4.9 use -std=c++11")
  add_definitions("-std=c++11")
elseif(GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7)
  message(STATUS "Version >= 4.7 and < 4.9 use -std=c++11 with boost regular expression")
  add_definitions("-std=c++11")
  add_defintions("-D ANCH_BOOST_REGEX")
  set(USE_BOOST_RE true)
else()
  message(STATUS "4.5 <= Version < 4.7 use -std=c++0x")
  add_definitions("-std=c++0x")
  add_defintions("-D ANCH_BOOST_REGEX")
  set(USE_BOOST_RE true)
endif()
# Check GCC version -

# Add GCC definitions
add_definitions("-Wall -Werror")

# Include librairies
include(FindThreads)
find_package(Threads)

# Includes location
include_directories(inc)
if(USE_BOOST_RE)
  find_package(Boost COMPONENTS regex)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

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
  SHARED
  ${source_files}
)

# Link with libraries
target_link_libraries(
  anch-resource
)

if(ANCH_RESOURCE_UNITARY_TEST)

  # Build parser unitary test +
  file(
    GLOB_RECURSE
    test_files
    inc/*
    test/testResource.cpp
  )

  add_executable(
    anch-resource-parse
    ${test_files}
  )

  if(USE_BOOST_RE)
    link_directories(${Boost_LIBRARY_DIRS})

    target_link_libraries(
      anch-resource-parse
      anch-resource
      ${CMAKE_THREAD_LIBS_INIT}
      ${Boost_LIBRARIES}
    )
  else()
    target_link_libraries(
      anch-resource-parse
      anch-resource
      ${CMAKE_THREAD_LIBS_INIT}
    )
  endif()
  # Build parser unitary test -

  enable_testing()
  add_test(anch-resource-parse ${EXECUTABLE_OUTPUT_PATH}/anch-resource-parse main)

endif(ANCH_RESOURCE_UNITARY_TEST)
