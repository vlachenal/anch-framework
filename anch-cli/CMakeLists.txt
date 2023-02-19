cmake_minimum_required(VERSION 3.0)

# Project anch CLI
# ANother C++ Harsh/Huge/Hack
project(anch-cli)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include(AnchCLI.cmake.in)

# Check GCC version +
execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                OUTPUT_VARIABLE GCC_VERSION)
if (GCC_VERSION VERSION_LESS 8.0)
  message(FATAL_ERROR "Version < 8.0. This library is based on C++17 standard. Exit.")
else()
  add_definitions("-std=c++20")
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
add_definitions(${ANCH_CLI_COMPILE_FLAGS})

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
  anch-cli-obj
  OBJECT
  ${source_files}
  )

## Dynamic library +
add_library(
  anch-cli
  SHARED
  $<TARGET_OBJECTS:anch-cli-obj>
)

install(
  DIRECTORY inc/ DESTINATION ${ANCH_CLI_INSTALL_INC_DIR}
  FILES_MATCHING PATTERN "*.hpp"
  )

install(
  TARGETS anch-cli
  LIBRARY DESTINATION ${ANCH_CLI_INSTALL_LIB_DIR}
  )
## Dynamic library -

## Static library +
if(ANCH_CLI_STATIC_LIB)
  add_library(
    anch-cli-static
    STATIC
    $<TARGET_OBJECTS:anch-cli-obj>
    )
  set_target_properties(anch-cli-static PROPERTIES OUTPUT_NAME anch-cli)

  install(
    TARGETS anch-cli-static
    ARCHIVE DESTINATION ${ANCH_CLI_INSTALL_LIB_DIR}
    )
endif(ANCH_CLI_STATIC_LIB)
## Static library -


# Unitary tests
if(ANCH_CLI_UNITARY_TEST)

  # Build CLI unit test +
  file(
    GLOB_RECURSE
    test_cli
    test/testCli.cpp
  )

  add_executable(
    anch-cli-test
    ${test_cli}
  )

  # Include libraries
  include(FindThreads)
  find_package(Threads)

  target_link_libraries(
    anch-cli-test
    anch-cli
    ${CMAKE_THREAD_LIBS_INIT}
  )
  # Build CLI unit test -

  enable_testing()
  add_test(anch-cli-test ${EXECUTABLE_OUTPUT_PATH}/anch-cli-test -h)

endif(ANCH_CLI_UNITARY_TEST)
