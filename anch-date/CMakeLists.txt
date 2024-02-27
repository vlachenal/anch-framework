cmake_minimum_required(VERSION 3.10)

# Project anch date
# ANother C++ Harsh/Huge/Hack
project(anch-date)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include(AnchDate.cmake.in)

# Check GCC version +
execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                OUTPUT_VARIABLE GCC_VERSION)
if (GCC_VERSION VERSION_LESS 8.0)
  message(FATAL_ERROR "Version < 8.0. This library is based on C++17 standard. Exit.")
elseif(GCC_VERSION VERSION_GREATER 10.0 OR GCC_VERSION VERSION_EQUAL 10.0)
  message(STATUS "Version >= 10.0 use -std=c++20")
  add_definitions("-std=c++20")
else()
  message(STATUS "Version >= 8.0 use -std=c++17")
  add_definitions("-std=c++17")
endif()
# Check GCC version -

# Add GCC definitions
add_definitions("-Wall -Wextra -Werror -Wuninitialized -Wconversion -Wsign-conversion -Wformat-security -fstack-protector-all -Wstack-protector --param ssp-buffer-size=4 -ftrapv  -z relro,now -fPIC -D_FORTIFY_SOURCE=2")
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  add_definitions("-D ANCH_WINDOWS")
else()
  add_definitions("-D ANCH_POSIX")
endif()
add_definitions(${ANCH_DATE_COMPILE_FLAGS})

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
  anch-date-obj
  OBJECT
  ${source_files}
  )

## Dynamic library +
add_library(
  anch-date
  SHARED
  $<TARGET_OBJECTS:anch-date-obj>
  )

install(
  DIRECTORY inc/ DESTINATION ${ANCH_DATE_INSTALL_INC_DIR}
  FILES_MATCHING PATTERN "*.hpp"
  )

install(
  TARGETS anch-date
  LIBRARY DESTINATION ${ANCH_DATE_INSTALL_LIB_DIR}
  )
## Dynamic library -

## Static library +
if(ANCH_DATE_STATIC_LIB)
  add_library(
    anch-date-static
    STATIC
    $<TARGET_OBJECTS:anch-date-obj>
    )
  set_target_properties(anch-date-static PROPERTIES OUTPUT_NAME anch-date)

  install(
    TARGETS anch-date-static
    ARCHIVE DESTINATION ${ANCH_DATE_INSTALL_LIB_DIR}
    )
endif(ANCH_DATE_STATIC_LIB)
## Static library -

# Unitary tests
if(ANCH_DATE_UNITARY_TEST)

  # Build date format unitary test +
  file(
    GLOB_RECURSE
    test_date_format
    test/testDateFormat.cpp
  )

  add_executable(
    anch-date-format-test
    ${test_date_format}
    )

  target_link_libraries(
    anch-date-format-test
    anch-date
    )
  # Build date format unitary test -

  enable_testing()
  add_test(anch-date-format-test ${EXECUTABLE_OUTPUT_PATH}/anch-date-format-test main)

endif(ANCH_DATE_UNITARY_TEST)
