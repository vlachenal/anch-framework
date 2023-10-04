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
if (GCC_VERSION VERSION_LESS 4.5)
  message(FATAL_ERROR "Version < 4.5. This library is based on C++11 standard. Exit.")
elseif(GCC_VERSION VERSION_GREATER 10.0 OR GCC_VERSION VERSION_EQUAL 10.0)
  message(STATUS "Version >= 10.0 use -std=c++20")
  add_definitions("-std=c++20")
elseif(GCC_VERSION VERSION_GREATER 8.0 OR GCC_VERSION VERSION_EQUAL 8.0)
  message(STATUS "Version >= 8.0 use -std=c++17")
  add_definitions("-std=c++17")
elseif(GCC_VERSION VERSION_GREATER 6.0 OR GCC_VERSION VERSION_EQUAL 6.0)
  message(STATUS "Version >= 6.0 use -std=c++14")
  add_definitions("-std=c++14")
elseif(GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7)
  message(STATUS "Version >= 4.7 use -std=c++11")
  add_definitions("-std=c++11")
else()
  message(STATUS "4.5 <= Version < 4.7 use -std=c++0x")
  add_definitions("-std=c++0x")
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

# Include librairies
include(FindThreads)
find_package(Threads)

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
    inc
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

  # Include libraries
  include(FindThreads)
  find_package(Threads)

  target_link_libraries(
    anch-uuid-test
    anch-uuid
    anch-cli
    anch-unit
    anch-json
    ${CMAKE_THREAD_LIBS_INIT}
  )
  # Build UUID unit test -

  enable_testing()
  add_test(anch-uuid-single-test ${EXECUTABLE_OUTPUT_PATH}/anch-uuid-test -t uuid-single)
  add_test(anch-uuid-collision-test ${EXECUTABLE_OUTPUT_PATH}/anch-uuid-test -t uuid-collision)
  add_test(anch-uuid-perf-test ${EXECUTABLE_OUTPUT_PATH}/anch-uuid-test -t uuid-perf)

endif(ANCH_UUID_UNITARY_TEST)
