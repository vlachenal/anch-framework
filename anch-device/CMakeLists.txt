cmake_minimum_required(VERSION 2.6)

# Project anch device
# ANother C++ Harsh/Huge/Hack
project(anch-device)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include(AnchDevice.cmake.in)

# Check GCC version +
execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                OUTPUT_VARIABLE GCC_VERSION)
if (GCC_VERSION VERSION_LESS 4.5)
  message(FATAL_ERROR "Version < 4.5. This library is based on C++11 standard. Exit.")
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
#add_definitions("-Wall -Wextra -Werror -Wuninitialized -Wconversion -Wsign-conversion -Wformat-security -fstack-protector-all -Wstack-protector --param ssp-buffer-size=4 -ftrapv  -z relro,now -fPIC -D_FORTIFY_SOURCE=2")
add_definitions("-Wall -Wextra -Werror -Wuninitialized")
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  add_definitions("-D ANCH_WINDOWS")
else()
  add_definitions("-D ANCH_POSIX")
endif()
add_definitions(${ANCH_DEVICE_COMPILE_FLAGS})

# Includes location
include_directories(
  inc
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
  anch-device
  SHARED
  ${source_files}
)


if(ANCH_DEVICE_UNITARY_TEST)

  # Build parser unitary test +
  # Valid network interface +
  file(
    GLOB_RECURSE
    test_valid_net_files
    inc/*
    test/testValidNetworkInterface.cpp
  )

  add_executable(
    anch-device-valid-network-interface
    ${test_valid_net_files}
  )

  target_link_libraries(
    anch-device-valid-network-interface
    anch-device
  )
  # Valid network interface -

  # Invalid network interface +
  file(
    GLOB_RECURSE
    test_invalid_net_files
    inc/*
    test/testInvalidNetworkInterface.cpp
    )

  add_executable(
    anch-device-invalid-network-interface
    ${test_invalid_net_files}
    )

  target_link_libraries(
    anch-device-invalid-network-interface
    anch-device
    )
  # Invalid network interface -

  # CPU instructions set +
  file(
    GLOB_RECURSE
    test_cpu
    #${ANCH_UTILS_INCLUDES_DIR}/*
    #inc/*
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
  # CPU instructions set -
  # Build parser unitary test -

  enable_testing()
  add_test(anch-device-valid-network-interface ${EXECUTABLE_OUTPUT_PATH}/anch-device-valid-network-interface main)
  add_test(anch-device-invalid-network-interface ${EXECUTABLE_OUTPUT_PATH}/anch-device-invalid-network-interface main)
  add_test(anch-device-cpu ${EXECUTABLE_OUTPUT_PATH}/anch-device-cpu main)

endif(ANCH_DEVICE_UNITARY_TEST)
