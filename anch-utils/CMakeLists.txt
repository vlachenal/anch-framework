cmake_minimum_required(VERSION 2.6)

# Project anch utils
# ANother C++ Harsh/Huge/Hack
project(anch-utils)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include(AnchUtils.cmake.in)

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
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  add_definitions("-D ANCH_WINDOWS")
else()
  add_definitions("-D ANCH_POSIX")
endif()
add_definitions(${ANCH_UTILS_COMPILE_FLAGS})

# Includes location
include_directories(
  inc
  ${ANCH_DEVICE_INCLUDES_DIR}
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
  anch-utils
  SHARED
  ${source_files}
)

# Unitary tests
if(ANCH_UTILS_UNITARY_TEST)

  link_directories(
    ${ANCH_DEVICE_LIBRARY_DIR}
  )

  # Build UUID unitary test +
  file(
    GLOB_RECURSE
    test_uuid
    inc/*
    test/testUuid.cpp
  )

  add_executable(
    anch-uuid-test
    ${test_uuid}
  )

  target_link_libraries(
    anch-uuid-test
    anch-utils
    anch-device
  )
  # Build UUID unitary test -

  enable_testing()
  add_test(anch-uuid-test ${EXECUTABLE_OUTPUT_PATH}/anch-uuid-test main)

endif(ANCH_UTILS_UNITARY_TEST)