cmake_minimum_required(VERSION 3.10)

# Project anch resource
# ANother C++ Harsh/Huge/Hack
project(anch-resource)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include(AnchResource.cmake.in)

# Check GCC version +
execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
if (GCC_VERSION VERSION_LESS 10.0)
  message(FATAL_ERROR "Version < 10.0. This library is based on C++20 standard. Exit.")
else()
  message(STATUS "Version >= 10.0 use -std=c++20")
  add_definitions("-std=c++20")
endif()
# Check GCC version -

# Add GCC definitions
add_definitions("-Wall -Wextra -Werror -Wuninitialized -Wconversion -Wsign-conversion -Wformat-security -fstack-protector-all -Wstack-protector --param ssp-buffer-size=4 -ftrapv  -z relro,now -fPIC -D_FORTIFY_SOURCE=2")
add_definitions(${ANCH_RESOURCE_COMPILE_FLAGS})

# Includes location
include_directories(inc)

# Retrieve every files in project
file(
  GLOB_RECURSE
  source_files
  src/*
  )

# Build objects
add_library(
  anch-resource-obj
  OBJECT
  ${source_files}
  )

## Dynamic library +
add_library(
  anch-resource
  SHARED
  $<TARGET_OBJECTS:anch-resource-obj>
  )

install(
  DIRECTORY inc/ DESTINATION ${ANCH_RESOURCE_INSTALL_INC_DIR}
  FILES_MATCHING PATTERN "*.hpp"
  )

install(
  TARGETS anch-resource
  LIBRARY DESTINATION ${ANCH_RESOURCE_INSTALL_LIB_DIR}
  )
## Dynamic library -

## Static library +
if(ANCH_RESOURCE_STATIC_LIB)
  add_library(
    anch-resource-static
    STATIC
    $<TARGET_OBJECTS:anch-resource-obj>
    )
  set_target_properties(anch-resource-static PROPERTIES OUTPUT_NAME anch-resource)

  install(
    TARGETS anch-resource-static
    ARCHIVE DESTINATION ${ANCH_RESOURCE_INSTALL_LIB_DIR}
    )
endif(ANCH_RESOURCE_STATIC_LIB)
## Static library -


if(ANCH_RESOURCE_UNITARY_TEST)

  include_directories(
    inc
    ${ANCH_CLI_INCLUDES_DIR}
    ${ANCH_UT_INCLUDES_DIR}
    )

  link_directories(
    ${ANCH_CLI_LIBRARY_DIR}
    ${ANCH_UT_LIBRARY_DIR}
    )

  configure_file(test/test.ini test.ini COPYONLY)
  # Build parser unitary test +
  file(
    GLOB_RECURSE
    test_files
    test/testResource.cpp
    )

  add_executable(
    anch-resource-parse
    ${test_files}
    )

  target_link_libraries(
    anch-resource-parse
    anch-resource
    anch-cli
    anch-unit
    )
  # Build parser unitary test -

  enable_testing()
  add_test(anch-resource-get-TOTO-toto ${EXECUTABLE_OUTPUT_PATH}/anch-resource-parse -t get-TOTO-toto)
  add_test(anch-resource-get-opt-TOTO-toto ${EXECUTABLE_OUTPUT_PATH}/anch-resource-parse -t get-opt-TOTO-toto)
  add_test(anch-resource-get-opt-tata ${EXECUTABLE_OUTPUT_PATH}/anch-resource-parse -t get-opt-tata)
  add_test(anch-resource-get-opt-toto ${EXECUTABLE_OUTPUT_PATH}/anch-resource-parse -t get-opt-toto)
  add_test(anch-resource-get-tata ${EXECUTABLE_OUTPUT_PATH}/anch-resource-parse -t get-tata)
  add_test(anch-resource-get-toto ${EXECUTABLE_OUTPUT_PATH}/anch-resource-parse -t get-toto)

endif(ANCH_RESOURCE_UNITARY_TEST)
