cmake_minimum_required(VERSION 3.10)

# Project anch network
# ANother C++ Harsh/Huge/Hack
project(anch-network)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include(AnchNetwork.cmake.in)

# Check GCC version +
execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
  OUTPUT_VARIABLE GCC_VERSION)
if (GCC_VERSION VERSION_LESS 9.3)
  message(FATAL_ERROR "Version < 10.0. This library is based on C++20 standard. Exit.")
else()
  message(STATUS "Version >= 10.0 use -std=c++20")
  add_definitions("-std=c++20")
endif()
# Check GCC version -

# Add GCC definitions
add_definitions("-Wall -Wextra -Werror -Wuninitialized -Wconversion -Wsign-conversion -Wformat-security -fstack-protector-all -Wstack-protector --param ssp-buffer-size=4 -ftrapv  -z relro,now -fPIC -D_FORTIFY_SOURCE=2")
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  add_definitions("-D ANCH_WINDOWS")
else()
  add_definitions("-D ANCH_POSIX")
endif()
add_definitions(${ANCH_NETWORK_COMPILE_FLAGS})

# Includes location
include_directories(
  inc
  ${ANCH_EVENTS_INCLUDES_DIR}
  ${ANCH_UTILS_INCLUDES_DIR}
  ${ANCH_CUTILS_INCLUDES_DIR}
  )

# Retrieve every files in project
file(
  GLOB_RECURSE
  source_files
  src/*
  )

# Build objects
add_library(
  anch-network-obj
  OBJECT
  ${source_files}
  )

## Dynamic library +
add_library(
  anch-network
  SHARED
  $<TARGET_OBJECTS:anch-network-obj>
  )

install(
  DIRECTORY inc/ DESTINATION ${ANCH_NETWORK_INSTALL_INC_DIR}
  FILES_MATCHING PATTERN "*.hpp"
  )

install(
  TARGETS anch-network
  LIBRARY DESTINATION ${ANCH_NETWORK_INSTALL_LIB_DIR}
  )
## Dynamic library -

## Static library +
if(ANCH_NETWORK_STATIC_LIB)
  add_library(
    anch-network-static
    STATIC
    $<TARGET_OBJECTS:anch-network-obj>
    )
  set_target_properties(anch-network-static PROPERTIES OUTPUT_NAME anch-network)

  install(
    TARGETS anch-network-static
    ARCHIVE DESTINATION ${ANCH_NETWORK_INSTALL_LIB_DIR}
    )
endif(ANCH_NETWORK_STATIC_LIB)
## Static library -

if(ANCH_NETWORK_UNITARY_TEST)

  include_directories(
    inc
    ${ANCH_EVENTS_INCLUDES_DIR}
    ${ANCH_UTILS_INCLUDES_DIR}
    ${ANCH_CUTILS_INCLUDES_DIR}
    ${ANCH_UT_INCLUDES_DIR}
    )

  link_directories(
    ${ANCH_CUTILS_LIBRARY_DIR}
    ${ANCH_CLI_LIBRARY_DIR}
    ${ANCH_UT_LIBRARY_DIR}
    )

  ## Build network unitary test +
  # TCP socket tests +
  file(
    GLOB_RECURSE
    test_tcp_files
    test/tcpSocketTest.cpp
    )

  add_executable(
    anch-network-test-tcp
    ${test_tcp_files}
    )

  target_link_libraries(
    anch-network-test-tcp
    anch-cutils
    anch-network
    )
  # TCP socket tests -

  # UDP socket tests +
  file(
    GLOB_RECURSE
    test_udp_files
    test/udpSocketTest.cpp
    )

  add_executable(
    anch-network-test-udp
    ${test_udp_files}
    )

  target_link_libraries(
    anch-network-test-udp
    anch-cutils
    anch-network
    )
  # UDP socket tests -

  # TCP stream tests +
  file(
    GLOB_RECURSE
    test_tcp_stream_files
    test/tcpStreamTest.cpp
    )

  add_executable(
    anch-network-test-tcp-stream
    ${test_tcp_stream_files}
    )

  target_link_libraries(
    anch-network-test-tcp-stream
    anch-cutils
    anch-cli
    anch-unit
    anch-network
    )
  # TCP stream tests -
  ## Build network unitary test -

  enable_testing()
  add_test(anch-network-test-tcp ${EXECUTABLE_OUTPUT_PATH}/anch-network-test-tcp main)
  add_test(anch-network-test-udp ${EXECUTABLE_OUTPUT_PATH}/anch-network-test-udp main)
  #add_test(anch-network-test-tcp-stream ${EXECUTABLE_OUTPUT_PATH}/anch-network-test-tcp-stream)

endif(ANCH_NETWORK_UNITARY_TEST)
