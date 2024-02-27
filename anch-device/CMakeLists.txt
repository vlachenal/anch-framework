cmake_minimum_required(VERSION 3.10)

# Project anch device
# ANother C++ Harsh/Huge/Hack
project(anch-device)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include(AnchDevice.cmake.in)

# Check GCC version +
execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                OUTPUT_VARIABLE GCC_VERSION)
if (GCC_VERSION VERSION_LESS 8.0)
  message(FATAL_ERROR "Version < 8.0. This library is based on C++17 standard. Exit.")
elseif(GCC_VERSION VERSION_GREATER 10.0 OR GCC_VERSION VERSION_EQUAL 10.0)
  message(STATUS "Version >= 10.0 use -std=c++20")
  add_definitions("-std=c++20")
elseif(GCC_VERSION VERSION_GREATER 8.0 OR GCC_VERSION VERSION_EQUAL 8.0)
  message(STATUS "Version >= 8.0 use -std=c++17")
  add_definitions("-std=c++17")
endif()
# Check GCC version -

# Add GCC definitions
add_definitions("-Wall -Wextra -Werror -Wuninitialized -Wconversion -Wsign-conversion -Wformat-security -fstack-protector-all -Wstack-protector --param ssp-buffer-size=4 -ftrapv  -z relro,now -fPIC -D_FORTIFY_SOURCE=2")
add_definitions(${ANCH_DEVICE_COMPILE_FLAGS})
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  add_definitions("-D ANCH_WINDOWS")
else()
  add_definitions("-D ANCH_POSIX")
endif()

# Includes location
include_directories(
  inc
  ${ANCH_UUID_INCLUDES_DIR}
)

# Retrieve every files in project
file(
  GLOB_RECURSE
  source_files
  src/*
)

# Build objects
add_library(
  anch-device-obj
  OBJECT
  ${source_files}
  )

## Dynamic library +
add_library(
  anch-device
  SHARED
  $<TARGET_OBJECTS:anch-device-obj>
  )

install(
  DIRECTORY inc/ DESTINATION ${ANCH_DEVICE_INSTALL_INC_DIR}
  FILES_MATCHING PATTERN "*.hpp"
  )

install(
  TARGETS anch-device
  LIBRARY DESTINATION ${ANCH_DEVICE_INSTALL_LIB_DIR}
  )
## Dynamic library -

## Static library +
if(ANCH_DEVICE_STATIC_LIB)
  add_library(
    anch-device-static
    STATIC
    $<TARGET_OBJECTS:anch-device-obj>
    )
  set_target_properties(anch-device-static PROPERTIES OUTPUT_NAME anch-device)

  install(
    TARGETS anch-device-static
    ARCHIVE DESTINATION ${ANCH_DEVICE_INSTALL_LIB_DIR}
    )
endif(ANCH_DEVICE_STATIC_LIB)
## Static library -


if(ANCH_DEVICE_UNITARY_TEST)

  link_directories(
    ${ANCH_UUID_LIBRARY_DIR}
    )

  # Build parser unitary test +
  # Valid network interface +
  file(
    GLOB_RECURSE
    test_valid_net_files
    test/testValidNetworkInterface.cpp
  )

  add_executable(
    anch-device-valid-network-interface
    ${test_valid_net_files}
  )

  target_link_libraries(
    anch-device-valid-network-interface
    anch-device
    anch-uuid
  )
  # Valid network interface -

  # Invalid network interface +
  file(
    GLOB_RECURSE
    test_invalid_net_files
    test/testInvalidNetworkInterface.cpp
    )

  add_executable(
    anch-device-invalid-network-interface
    ${test_invalid_net_files}
    )

  target_link_libraries(
    anch-device-invalid-network-interface
    anch-device
    anch-uuid
    )
  # Invalid network interface -

  # CPU instructions set +
  file(
    GLOB_RECURSE
    test_cpu
    test/testCPU.cpp
    )

  # Includes location
  include_directories(
    inc
    ${ANCH_UTILS_INCLUDES_DIR}
    )

  add_executable(
    anch-device-cpu
    ${test_cpu}
    )

  target_link_libraries(
    anch-device-cpu
    anch-device
    anch-uuid
    )
  # CPU instructions set -

  # UUID +
  file(
    GLOB_RECURSE
    test_uuid
    test/testUuid.cpp
    )

  # Includes location
  include_directories(
    inc
    ${ANCH_UUID_INCLUDES_DIR}
    )

  add_executable(
    anch-device-uuid
    ${test_uuid}
    )

  target_link_libraries(
    anch-device-uuid
    anch-device
    anch-uuid
    )
  # UUID -
  # Build parser unitary test -

  enable_testing()
  add_test(anch-device-valid-network-interface ${EXECUTABLE_OUTPUT_PATH}/anch-device-valid-network-interface main)
  add_test(anch-device-invalid-network-interface ${EXECUTABLE_OUTPUT_PATH}/anch-device-invalid-network-interface main)
  add_test(anch-device-cpu ${EXECUTABLE_OUTPUT_PATH}/anch-device-cpu main)
  add_test(anch-device-uuid ${EXECUTABLE_OUTPUT_PATH}/anch-device-uuid main)

endif(ANCH_DEVICE_UNITARY_TEST)
