cmake_minimum_required(VERSION 3.10)

# Project anch csv
# ANother C++ Harsh/Huge/Hack
project(anch-csv)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include(AnchCsv.cmake.in)

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
add_definitions(${ANCH_CSV_COMPILE_FLAGS})

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
  anch-csv-obj
  OBJECT
  ${source_files}
)

## Dynamic library +
add_library(
  anch-csv
  SHARED
  $<TARGET_OBJECTS:anch-csv-obj>
)

install(
  DIRECTORY inc/ DESTINATION ${ANCH_INSTALL_INC_DIR}
  FILES_MATCHING PATTERN "*.hpp"
)

install(
  TARGETS anch-csv
  LIBRARY DESTINATION ${ANCH_INSTALL_LIB_DIR}
)
## Dynamic library -

## Static library +
if(ANCH_CSV_STATIC_LIB)
  add_library(
    anch-csv-static
    STATIC
    $<TARGET_OBJECTS:anch-csv-obj>
  )
  set_target_properties(anch-csv-static PROPERTIES OUTPUT_NAME anch-csv)

  install(
    TARGETS anch-csv-static
    ARCHIVE DESTINATION ${ANCH_INSTALL_LIB_DIR}
  )
endif(ANCH_CSV_STATIC_LIB)
## Static library -

# Unitary tests
if(ANCH_CSV_UNITARY_TEST)

endif(ANCH_CSV_UNITARY_TEST)
