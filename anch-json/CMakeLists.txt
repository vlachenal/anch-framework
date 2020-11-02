cmake_minimum_required(VERSION 2.6)

# Project anch json
# ANother C++ Harsh/Huge/Hack
project(anch-json)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include(AnchJson.cmake.in)

# Check GCC version +
execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                OUTPUT_VARIABLE GCC_VERSION)
if (GCC_VERSION VERSION_LESS 4.5)
  message(FATAL_ERROR "Version < 4.5. This library is based on C++11 standard. Exit.")
elseif(GCC_VERSION VERSION_GREATER 10.0 OR GCC_VERSION VERSION_EQUAL 10.0)
  message(STATUS "Version >= 10.0 use -std=c++20")
  add_definitions("-std=c++20")
elseif(GCC_VERSION VERSION_GREATER 9.0 OR GCC_VERSION VERSION_EQUAL 9.0)
  message(STATUS "Version >= 9.0 use -std=c++2a")
  add_definitions("-std=c++2a -fconcepts")
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
add_definitions(${ANCH_JSON_COMPILE_FLAGS})

# Includes location
include_directories(
  inc
  ${ANCH_UUID_INCLUDES_DIR}
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
  anch-json-obj
  OBJECT
  ${source_files}
  )

## Dynamic library +
add_library(
  anch-json
  SHARED
  $<TARGET_OBJECTS:anch-json-obj>
)

install(
  DIRECTORY inc/ DESTINATION ${ANCH_JSON_INSTALL_INC_DIR}
  FILES_MATCHING PATTERN "*.hpp"
  )

install(
  TARGETS anch-json
  LIBRARY DESTINATION ${ANCH_JSON_INSTALL_LIB_DIR}
  )
## Dynamic library -

## Static library +
if(ANCH_JSON_STATIC_LIB)
  add_library(
    anch-json-static
    STATIC
    $<TARGET_OBJECTS:anch-json-obj>
    )
  set_target_properties(anch-json-static PROPERTIES OUTPUT_NAME anch-json)

  install(
    TARGETS anch-json-static
    ARCHIVE DESTINATION ${ANCH_JSON_INSTALL_LIB_DIR}
    )
endif(ANCH_JSON_STATIC_LIB)
## Static library -


# Unitary tests
if(ANCH_JSON_UNITARY_TEST)
  configure_file(test/toto.json toto.json COPYONLY)
  configure_file(test/totos.json totos.json COPYONLY)

  link_directories(
    ${ANCH_UUID_LIBRARY_DIR}
    )

  # Build JSON serialization unit tests +
  file(
    GLOB_RECURSE
    test_serialize
    test/testSerialize.cpp
  )

  add_executable(
    anch-json-serializer-test
    ${test_serialize}
  )

  target_link_libraries(
    anch-json-serializer-test
    anch-json
    anch-uuid
  )
  # Build JSON serialization unit tests -

  enable_testing()

  add_test(json-serializer ${EXECUTABLE_OUTPUT_PATH}/anch-json-serializer-test main)

endif(ANCH_JSON_UNITARY_TEST)
