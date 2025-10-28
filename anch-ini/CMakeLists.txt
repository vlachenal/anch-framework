cmake_minimum_required(VERSION 3.10)

# Project anch ini
# ANother C++ Harsh/Huge/Hack
project(anch-ini)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include(AnchIni.cmake.in)

# Check GCC version +
execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
  OUTPUT_VARIABLE GCC_VERSION)
if (GCC_VERSION VERSION_LESS 13.0)
  message(FATAL_ERROR "Version < 13.0. This library is based on C++23 standard. Exit.")
else()
  message(STATUS "Version >= 13.0 use -std=c++23")
  add_definitions("-std=c++23")
endif()
# Check GCC version -

# Add GCC definitions
add_definitions("-Wall -Wextra -Werror -Wuninitialized -Wconversion -Wsign-conversion -Wformat-security -fstack-protector-all -Wstack-protector --param ssp-buffer-size=4 -ftrapv -D_FORTIFY_SOURCE=2 -z relro,now -fPIC")
#add_definitions("-Wall -Wextra -Werror -Wuninitialized -Wconversion -Wsign-conversion -Wformat-security -fstack-protector-all -Wstack-protector --param ssp-buffer-size=4 -ftrapv -z relro,now -fPIC")
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  add_definitions("-D ANCH_WINDOWS")
else()
  add_definitions("-D ANCH_POSIX")
endif()
add_definitions(${ANCH_INI_COMPILE_FLAGS})

if(ANCH_UUID)
  add_definitions("-D ANCH_UUID")
endif()

# Includes location
include_directories(
  inc
  ${ANCH_UTILS_INCLUDES_DIR}
  ${ANCH_EVENTS_INCLUDES_DIR}
)
if(ANCH_UUID)
  include_directories(
    ${ANCH_UUID_INCLUDES_DIR}
  )
endif(ANCH_UUID)

# Retrieve every files in project
file(
  GLOB_RECURSE
  source_files
  src/*
)

# Build objects
add_library(
  anch-ini-obj
  OBJECT
  ${source_files}
)

## Dynamic library +
add_library(
  anch-ini
  SHARED
  $<TARGET_OBJECTS:anch-ini-obj>
)

install(
  DIRECTORY inc/ DESTINATION ${ANCH_INI_INSTALL_INC_DIR}
  FILES_MATCHING PATTERN "*.hpp"
)

install(
  TARGETS anch-ini
  LIBRARY DESTINATION ${ANCH_INI_INSTALL_LIB_DIR}
)
## Dynamic library -

## Static library +
if(ANCH_INI_STATIC_LIB)
  add_library(
    anch-ini-static
    STATIC
    $<TARGET_OBJECTS:anch-ini-obj>
  )
  set_target_properties(anch-ini-static PROPERTIES OUTPUT_NAME anch-ini)

  install(
    TARGETS anch-ini-static
    ARCHIVE DESTINATION ${ANCH_INI_INSTALL_LIB_DIR}
  )
endif(ANCH_INI_STATIC_LIB)
## Static library -


# Unitary tests
if(ANCH_INI_UNITARY_TEST)
  configure_file(test/test.ini test.ini COPYONLY)

  include_directories(
    ${ANCH_CLI_INCLUDES_DIR}
    ${ANCH_UUID_INCLUDES_DIR}
    ${ANCH_UT_INCLUDES_DIR}
  )
  link_directories(
    ${ANCH_CLI_LIBRARY_DIR}
    ${ANCH_UUID_LIBRARY_DIR}
    ${ANCH_UT_LIBRARY_DIR}
  )

  # Build INI parser unit tests +
  file(
    GLOB_RECURSE
    test_parser
    test/testParser.cpp
  )

  add_executable(
    anch-ini-parser-test
    ${test_parser}
  )

  target_link_libraries(
    anch-ini-parser-test
    anch-ini
    anch-cli
    anch-unit
  )
  if(ANCH_UUID)
    target_link_libraries(
      anch-ini-parser-test
      anch-uuid
    )
  endif(ANCH_UUID)
  # Build INI parser unit tests -

  enable_testing()

  # Parser unit tests +
  add_test(parser-root-val ${EXECUTABLE_OUTPUT_PATH}/anch-ini-parser-test -t parser-root-val)
  add_test(parser-root-sub-val ${EXECUTABLE_OUTPUT_PATH}/anch-ini-parser-test -t parser-root-sub-val)
  add_test(parser-root-sub-keysub-val ${EXECUTABLE_OUTPUT_PATH}/anch-ini-parser-test -t parser-root-sub-keysub-val)
  add_test(parser-root-subsub-val ${EXECUTABLE_OUTPUT_PATH}/anch-ini-parser-test -t parser-root-subsub-val)
  add_test(parser-int16 ${EXECUTABLE_OUTPUT_PATH}/anch-ini-parser-test -t parser-int16)
  add_test(parser-int32 ${EXECUTABLE_OUTPUT_PATH}/anch-ini-parser-test -t parser-int32)
  add_test(parser-int64 ${EXECUTABLE_OUTPUT_PATH}/anch-ini-parser-test -t parser-int64)
  add_test(parser-uint16 ${EXECUTABLE_OUTPUT_PATH}/anch-ini-parser-test -t parser-uint16)
  add_test(parser-uint32 ${EXECUTABLE_OUTPUT_PATH}/anch-ini-parser-test -t parser-uint32)
  add_test(parser-uint64 ${EXECUTABLE_OUTPUT_PATH}/anch-ini-parser-test -t parser-uint64)
  add_test(parser-float ${EXECUTABLE_OUTPUT_PATH}/anch-ini-parser-test -t parser-float)
  add_test(parser-double ${EXECUTABLE_OUTPUT_PATH}/anch-ini-parser-test -t parser-double)
  add_test(parser-bool ${EXECUTABLE_OUTPUT_PATH}/anch-ini-parser-test -t parser-bool)
  # Parser unit tests -

endif(ANCH_INI_UNITARY_TEST)
