cmake_minimum_required(VERSION 3.10)

# Project anch OAS
# ANother C++ Harsh/Huge/Hack
project(anch-oas)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include(AnchOAS.cmake.in)

# Check GCC version +
execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                OUTPUT_VARIABLE GCC_VERSION)
if (GCC_VERSION VERSION_LESS 10.0)
  message(FATAL_ERROR "Version < 10.0. This library is based on recent c++ standard. Exit.")
elseif(GCC_VERSION VERSION_GREATER 10.0 OR GCC_VERSION VERSION_EQUAL 10.0)
  message(STATUS "Version >= 10.0 use -std=c++20")
  add_definitions("-std=c++20")
endif()
# Check GCC version -

# Add GCC definitions
add_definitions("-Wall -Wextra -Werror -Wuninitialized -Wconversion -Wsign-conversion -Wformat-security -fstack-protector-all -Wstack-protector --param ssp-buffer-size=4 -ftrapv -z relro,now -fPIC -D_FORTIFY_SOURCE=2")
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  add_definitions("-D ANCH_WINDOWS")
else()
  add_definitions("-D ANCH_POSIX")
endif()
add_definitions(${ANCH_OAS_COMPILE_FLAGS})

# Includes location
include_directories(
  inc
  ${ANCH_CLI_INCLUDES_DIR}
  ${ANCH_JSON_INCLUDES_DIR}
  )

# Retrieve every files in project
file(
  GLOB_RECURSE
  source_files
  src/*
  )

# Build objects
add_library(
  anch-unit-obj
  OBJECT
  ${source_files}
  )

## Dynamic library +
add_library(
  anch-unit
  SHARED
  $<TARGET_OBJECTS:anch-unit-obj>
  )

install(
  DIRECTORY inc/ DESTINATION ${ANCH_OAS_INSTALL_INC_DIR}
  FILES_MATCHING PATTERN "*.hpp"
  )

install(
  TARGETS anch-unit
  LIBRARY DESTINATION ${ANCH_OAS_INSTALL_LIB_DIR}
  )
## Dynamic library -

## Static library +
if(ANCH_OAS_STATIC_LIB)
  add_library(
    anch-unit-static
    STATIC
    $<TARGET_OBJECTS:anch-unit-obj>
    )
  set_target_properties(anch-unit-static PROPERTIES OUTPUT_NAME anch-unit)

  install(
    TARGETS anch-unit-static
    ARCHIVE DESTINATION ${ANCH_OAS_INSTALL_LIB_DIR}
    )
endif(ANCH_OAS_STATIC_LIB)
## Static library -

# Unitary tests
if(ANCH_OAS_UNITARY_TEST)

  link_directories(
    ${ANCH_CLI_LIBRARY_DIR}
    )

  # Build unit format unitary test +
  file(
    GLOB_RECURSE
    test_unit
    test/testUnit.cpp
    )

  add_executable(
    anch-unit-test
    ${test_unit}
    )

  target_link_libraries(
    anch-unit-test
    anch-unit
    anch-cli
    ${CMAKE_THREAD_LIBS_INIT}
    )
  # Build unit format unitary test -

  enable_testing()
  add_test(anch-unit-test ${EXECUTABLE_OUTPUT_PATH}/anch-unit-test)
  add_test(anch-unit-test-all ${EXECUTABLE_OUTPUT_PATH}/anch-unit-test -t all)
  add_test(anch-unit-test-nothing ${EXECUTABLE_OUTPUT_PATH}/anch-unit-test -t nothing)
  add_test(anch-unit-test-exception ${EXECUTABLE_OUTPUT_PATH}/anch-unit-test -t exception)
  add_test(anch-unit-test-assert ${EXECUTABLE_OUTPUT_PATH}/anch-unit-test -t assert)

  set_tests_properties(anch-unit-test PROPERTIES WILL_FAIL TRUE)
  set_tests_properties(anch-unit-test-all PROPERTIES WILL_FAIL TRUE)
  set_tests_properties(anch-unit-test-exception PROPERTIES WILL_FAIL TRUE)
  set_tests_properties(anch-unit-test-assert PROPERTIES WILL_FAIL TRUE)

endif(ANCH_OAS_UNITARY_TEST)
