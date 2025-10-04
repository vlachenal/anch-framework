cmake_minimum_required(VERSION 3.10)

# Project anch UUID
# ANother C++ Harsh/Huge/Hack
project(anch-uuid)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include(AnchUUID.cmake.in)

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
add_definitions(${ANCH_UUID_COMPILE_FLAGS})

# Includes location
include_directories(
  inc
)

# Retrieve every files in project
file(
  GLOB_RECURSE
  source_files
  src/*
)

# Build objects
add_library(
  anch-uuid-obj
  OBJECT
  ${source_files}
  )

## Dynamic library +
add_library(
  anch-uuid
  SHARED
  $<TARGET_OBJECTS:anch-uuid-obj>
)

install(
  DIRECTORY inc/ DESTINATION ${ANCH_UUID_INSTALL_INC_DIR}
  FILES_MATCHING PATTERN "*.hpp"
  )

install(
  TARGETS anch-uuid
  LIBRARY DESTINATION ${ANCH_UUID_INSTALL_LIB_DIR}
  )
## Dynamic library -

## Static library +
if(ANCH_UUID_STATIC_LIB)
  add_library(
    anch-uuid-static
    STATIC
    $<TARGET_OBJECTS:anch-uuid-obj>
    )
  set_target_properties(anch-uuid-static PROPERTIES OUTPUT_NAME anch-uuid)

  install(
    TARGETS anch-uuid-static
    ARCHIVE DESTINATION ${ANCH_UUID_INSTALL_LIB_DIR}
    )
endif(ANCH_UUID_STATIC_LIB)
## Static library -


# Unitary tests
if(ANCH_UUID_UNITARY_TEST)

  include_directories(
    ${ANCH_UT_INCLUDES_DIR}
    )

  link_directories(
    ${ANCH_UUID_LIBRARY_DIR}
    ${ANCH_CLI_LIBRARY_DIR}
    ${ANCH_UT_LIBRARY_DIR}
    ${ANCH_JSON_LIBRARY_DIR}
    )

  # Build UUID unit test +
  file(
    GLOB_RECURSE
    test_uuid
    test/testUuid.cpp
  )

  add_executable(
    anch-uuid-test
    ${test_uuid}
  )

  target_link_libraries(
    anch-uuid-test
    anch-uuid
    anch-cli
    anch-unit
    anch-json
  )
  # Build UUID unit test -

  enable_testing()
  add_test(uuid-random ${EXECUTABLE_OUTPUT_PATH}/anch-uuid-test -t uuid-random)
  add_test(uuid-random-time ${EXECUTABLE_OUTPUT_PATH}/anch-uuid-test -t uuid-random-time)
  add_test(uuid-collision ${EXECUTABLE_OUTPUT_PATH}/anch-uuid-test -t uuid-collision)
  add_test(uuid-perf ${EXECUTABLE_OUTPUT_PATH}/anch-uuid-test -t uuid-perf)

endif(ANCH_UUID_UNITARY_TEST)
