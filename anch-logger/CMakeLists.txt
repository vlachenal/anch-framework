cmake_minimum_required(VERSION 2.6)

# Project anch logger
# ANother C++ Hack 
project(anch-logger)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include(AnchLogger.cmake.in)

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

find_package(Boost COMPONENTS regex)

# Includes location
include_directories(
  inc
  ${Boost_INCLUDE_DIRS}
  ${ANCH_RESOURCE_INCLUDES_DIR}
)

link_directories(
  ${Boost_LIBRARY_DIRS}
  ${ANCH_RESOURCE_LIBRARY_DIR}
)

# Retrieve every files in project
file(
  GLOB_RECURSE
  source_files
  src/*
  inc/*
)

# Declare exec
add_library(
  anch-logger
  SHARED
  ${source_files}
)

# Link with libraries
target_link_libraries(
  anch-logger
  ${CMAKE_THREAD_LIBS_INIT}
  ${Boost_LIBRARIES}
  anch-resource
)

# Unitary tests
if(ANCH_LOGGER_UNITARY_TEST)

  # build rotate test +
  file(
    GLOB_RECURSE
    test_rotate
    inc/*
    test/testRotate.cpp
  )

  add_executable(
    anch-logger-rotate
    ${test_rotate}
  )

  target_link_libraries(
    anch-logger-rotate
    anch-logger
  )
  # build rotate test -

  # build custom file test +
  file(
    GLOB_RECURSE
    test_custom_file
    inc/*
    test/testCustomFile.cpp
  )

  add_executable(
    anch-logger-custom-file
    ${test_custom_file}
  )

  list(APPEND custom_file_def "_ANCH_LOGGER_CONFIG_FILE_=\"/home/winz/tmp/customFile.ini\"")
  set_target_properties(
    anch-logger-custom-file
    PROPERTIES
    COMPILE_DEFINITIONS "${custom_file_def}"
  )

  target_link_libraries(
    anch-logger-custom-file
    anch-logger
  )
  # build rotate test -

  enable_testing()
  add_test(anch-logger-rotate ${EXECUTABLE_OUTPUT_PATH}/anch-logger-rotate main)
  add_test(anch-logger-custom-file ${EXECUTABLE_OUTPUT_PATH}/anch-logger-custom-file main)

endif(ANCH_LOGGER_UNITARY_TEST)
