cmake_minimum_required(VERSION 3.0)

# Project anch cutils
# ANother C++ Harsh/Huge/Hack
project(anch-cutils)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include(AnchCutils.cmake.in)

# Check GCC version +
execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                OUTPUT_VARIABLE GCC_VERSION)
if (GCC_VERSION VERSION_LESS 9.3)
  message(FATAL_ERROR "Version < 9.3. This library is based on C++20 standard. Exit.")
elseif(GCC_VERSION VERSION_GREATER 10.0 OR GCC_VERSION VERSION_EQUAL 10.0)
  message(STATUS "Version >= 10.0 use -std=c++20")
  add_definitions("-std=c++20")
elseif(GCC_VERSION VERSION_GREATER 9.3 OR GCC_VERSION VERSION_EQUAL 9.3)
  message(STATUS "Version >= 9.3 use -std=c++2a")
  add_definitions("-std=c++2a")
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
add_definitions(${ANCH_CUTILS_COMPILE_FLAGS})

# Include librairies
include(FindThreads)
find_package(Threads)

# Includes location
include_directories(
  inc
  ${ANCH_UTILS_INCLUDES_DIR}
  )

# Retrieve every files in project
file(
  GLOB_RECURSE
  source_files
  src/*
  )

# Build objects
add_library(
  anch-cutils-obj
  OBJECT
  ${source_files}
  )

## Dynamic library +
add_library(
  anch-cutils
  SHARED
  $<TARGET_OBJECTS:anch-cutils-obj>
)

install(
  DIRECTORY inc/ DESTINATION ${ANCH_CUTILS_INSTALL_INC_DIR}
  FILES_MATCHING PATTERN "*.hpp"
  )

install(
  TARGETS anch-cutils
  LIBRARY DESTINATION ${ANCH_CUTILS_INSTALL_LIB_DIR}
  )
## Dynamic library -

## Static library +
if(ANCH_CUTILS_STATIC_LIB)
  add_library(
    anch-cutils-static
    STATIC
    $<TARGET_OBJECTS:anch-cutils-obj>
    )
  set_target_properties(anch-cutils-static PROPERTIES OUTPUT_NAME anch-cutils)

  install(
    TARGETS anch-cutils-static
    ARCHIVE DESTINATION ${ANCH_CUTILS_INSTALL_LIB_DIR}
    )
endif(ANCH_CUTILS_STATIC_LIB)
## Static library -


# Unitary tests
if(ANCH_CUTILS_UNITARY_TEST)

  include_directories(
    inc
    ${ANCH_UTILS_INCLUDES_DIR}
    ${ANCH_JSON_INCLUDES_DIR}
    ${ANCH_UT_INCLUDES_DIR}
    ${ANCH_CLI_INCLUDES_DIR}
    )

  link_directories(
    ${ANCH_JSON_LIBRARY_DIR}
    ${ANCH_UT_LIBRARY_DIR}
    ${ANCH_CLI_LIBRARY_DIR}
    )

  # CStream unit tests +
  file(
    GLOB_RECURSE
    test_cstream
    test/testCStream.cpp
    )

  add_executable(
    anch-cstream-test
    ${test_cstream}
    )

  target_link_libraries(
    anch-cstream-test
    anch-cutils
    anch-json
    anch-unit
    anch-cli
    ${CMAKE_THREAD_LIBS_INIT}
    )
  # CStream unit tests -

  enable_testing()

  add_test(cstream-output-jsonser ${EXECUTABLE_OUTPUT_PATH}/anch-cstream-test --test serialize)
  add_test(cstream-output-jsondeser ${EXECUTABLE_OUTPUT_PATH}/anch-cstream-test --test deserialize)
  add_test(cstream-output-jsonser-1char ${EXECUTABLE_OUTPUT_PATH}/anch-cstream-test --test serialize1)
  add_test(cstream-output-jsondeser-1char ${EXECUTABLE_OUTPUT_PATH}/anch-cstream-test --test deserialize1)
  add_test(cstream-output-jsonser-1500char ${EXECUTABLE_OUTPUT_PATH}/anch-cstream-test --test serialize1500)
  add_test(cstream-output-jsondeser-1500char ${EXECUTABLE_OUTPUT_PATH}/anch-cstream-test --test deserialize1500)
  add_test(cstream-output-jsonser-25char ${EXECUTABLE_OUTPUT_PATH}/anch-cstream-test --test serialize25)
  add_test(cstream-output-jsondeser-25char ${EXECUTABLE_OUTPUT_PATH}/anch-cstream-test --test deserialize25)

endif(ANCH_CUTILS_UNITARY_TEST)
