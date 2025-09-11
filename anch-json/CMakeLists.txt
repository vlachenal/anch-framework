cmake_minimum_required(VERSION 3.10)

# Project anch json
# ANother C++ Harsh/Huge/Hack
project(anch-json)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include(AnchJson.cmake.in)

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
add_definitions(${ANCH_JSON_COMPILE_FLAGS})

if(${ANCH_UUID})
  add_definitions("-D ANCH_UUID")
endif()

# Includes location
include_directories(
  inc
  ${ANCH_UUID_INCLUDES_DIR}
  ${ANCH_UTILS_INCLUDES_DIR}
  ${ANCH_EVENTS_INCLUDES_DIR}
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


## Utilities +
if(ANCH_JSON_UNITARY_UTILS)
  include_directories(
    inc
    ${ANCH_EVENTS_INCLUDES_DIR}
    ${ANCH_JSON_INCLUDES_DIR}
    ${ANCH_CLI_INCLUDES_DIR}
    )

  link_directories(
    ${ANCH_CLI_LIBRARY_DIR}
    ${ANCH_UUID_LIBRARY_DIR}
  )

  file(
    GLOB_RECURSE
    json_formatter
    utils/jsonFormat.cpp
  )

  add_executable(
    anch-json-formatter
    ${json_formatter}
  )

  target_link_libraries(
    anch-json-formatter
    anch-json
    anch-cli
    anch-uuid
  )

  set_target_properties(
    anch-json-formatter
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/utils
  )
endif(ANCH_JSON_UNITARY_UTILS)
## Utilities -


# Unitary tests
if(ANCH_JSON_UNITARY_TEST)
  configure_file(test/toto.json toto.json COPYONLY)
  configure_file(test/totos.json totos.json COPYONLY)
  configure_file(test/data_set.json data_set.json COPYONLY)

  include_directories(
    inc
    ${ANCH_EVENTS_INCLUDES_DIR}
    ${ANCH_UTILS_INCLUDES_DIR}
    ${ANCH_JSON_INCLUDES_DIR}
    ${ANCH_CLI_INCLUDES_DIR}
    ${ANCH_UT_INCLUDES_DIR}
    ${ANCH_UUID_INCLUDES_DIR}
    )

  link_directories(
    ${ANCH_UUID_LIBRARY_DIR}
    ${ANCH_CLI_LIBRARY_DIR}
    ${ANCH_UT_LIBRARY_DIR}
    ${ANCH_UUID_LIBRARY_DIR}
    )

  # Build JSON lexer unit tests +
  file(
    GLOB_RECURSE
    test_lexer
    test/testLexer.cpp
  )

  add_executable(
    anch-json-lexer-test
    ${test_lexer}
  )

  target_link_libraries(
    anch-json-lexer-test
    anch-json
    anch-cli
    anch-unit
    anch-uuid
  )
  # Build JSON lexer unit tests -

  # Build JSON mapper unit tests +
  file(
    GLOB_RECURSE
    test_mapper
    test/testJsonMapper.cpp
  )

  add_executable(
    anch-json-mapper-test
    ${test_mapper}
  )

  target_link_libraries(
    anch-json-mapper-test
    anch-json
    anch-cli
    anch-unit
    anch-uuid
  )
  # Build JSON mapper unit tests -

  # Build JSON reader unit tests +
  file(
    GLOB_RECURSE
    test_reader
    test/testReader.cpp
  )

  add_executable(
    anch-json-reader
    ${test_reader}
  )

  target_link_libraries(
    anch-json-reader
    anch-json
    anch-cli
    anch-unit
    anch-uuid
  )
  # Build JSON reader unit tests -

  # Build JSON bench unit tests +
  file(
    GLOB_RECURSE
    test_bench
    test/benchJson.cpp
  )

  add_executable(
    anch-json-bench
    ${test_bench}
  )

  target_link_libraries(
    anch-json-bench
    anch-json
    anch-cli
    anch-uuid
  )
  # Build JSON bench unit tests -

  enable_testing()

  # Lexer unit tests +
  # boolean
  add_test(lexer-bool-false ${EXECUTABLE_OUTPUT_PATH}/anch-json-lexer-test -t lexer-bool-false)
  add_test(lexer-bool-true ${EXECUTABLE_OUTPUT_PATH}/anch-json-lexer-test -t lexer-bool-true)
  add_test(lexer-bool-null ${EXECUTABLE_OUTPUT_PATH}/anch-json-lexer-test -t lexer-bool-null)
  # string
  add_test(lexer-str ${EXECUTABLE_OUTPUT_PATH}/anch-json-lexer-test -t lexer-str)
  add_test(lexer-str-null ${EXECUTABLE_OUTPUT_PATH}/anch-json-lexer-test -t lexer-str-null)
  # uint
  add_test(lexer-uint ${EXECUTABLE_OUTPUT_PATH}/anch-json-lexer-test -t lexer-uint)
  add_test(lexer-uint-null ${EXECUTABLE_OUTPUT_PATH}/anch-json-lexer-test -t lexer-uint-null)
  add_test(lexer-uint-0-ko ${EXECUTABLE_OUTPUT_PATH}/anch-json-lexer-test -t lexer-uint-0-ko)
  add_test(lexer-uint-s-ko ${EXECUTABLE_OUTPUT_PATH}/anch-json-lexer-test -t lexer-uint-s-ko)
  # int
  add_test(lexer-int ${EXECUTABLE_OUTPUT_PATH}/anch-json-lexer-test -t lexer-int)
  add_test(lexer-int-null ${EXECUTABLE_OUTPUT_PATH}/anch-json-lexer-test -t lexer-int-null)
  add_test(lexer-int-0-ko ${EXECUTABLE_OUTPUT_PATH}/anch-json-lexer-test -t lexer-int-0-ko)
  # double
  add_test(lexer-double-u ${EXECUTABLE_OUTPUT_PATH}/anch-json-lexer-test -t lexer-double-u)
  add_test(lexer-double-u-dot ${EXECUTABLE_OUTPUT_PATH}/anch-json-lexer-test -t lexer-double-u-dot)
  add_test(lexer-double-u-0-dot ${EXECUTABLE_OUTPUT_PATH}/anch-json-lexer-test -t lexer-double-u-0-dot)
  add_test(lexer-double-s ${EXECUTABLE_OUTPUT_PATH}/anch-json-lexer-test -t lexer-double-s)
  add_test(lexer-double-s-dot ${EXECUTABLE_OUTPUT_PATH}/anch-json-lexer-test -t lexer-double-s-dot)
  add_test(lexer-double-s-0-dot ${EXECUTABLE_OUTPUT_PATH}/anch-json-lexer-test -t lexer-double-s-0-dot)
  add_test(lexer-double-null ${EXECUTABLE_OUTPUT_PATH}/anch-json-lexer-test -t lexer-double-null)
  add_test(lexer-double-u-exp ${EXECUTABLE_OUTPUT_PATH}/anch-json-lexer-test -t lexer-double-u-exp)
  add_test(lexer-double-s-exp ${EXECUTABLE_OUTPUT_PATH}/anch-json-lexer-test -t lexer-double-s-exp)
  add_test(lexer-double-u-dot-exp ${EXECUTABLE_OUTPUT_PATH}/anch-json-lexer-test -t lexer-double-u-dot-exp)
  add_test(lexer-double-s-dot-exp ${EXECUTABLE_OUTPUT_PATH}/anch-json-lexer-test -t lexer-double-s-dot-exp)
  # array
  add_test(lexer-array-null ${EXECUTABLE_OUTPUT_PATH}/anch-json-lexer-test -t lexer-array-null)
  add_test(lexer-array-empty ${EXECUTABLE_OUTPUT_PATH}/anch-json-lexer-test -t lexer-array-empty)
  add_test(lexer-array-list ${EXECUTABLE_OUTPUT_PATH}/anch-json-lexer-test -t lexer-array-list)
  add_test(lexer-array-vector ${EXECUTABLE_OUTPUT_PATH}/anch-json-lexer-test -t lexer-array-vector)
  add_test(lexer-array-set ${EXECUTABLE_OUTPUT_PATH}/anch-json-lexer-test -t lexer-array-set)
  # Lexer unit tests -

  # Mapper unit tests +
  add_test(json-ser-full ${EXECUTABLE_OUTPUT_PATH}/anch-json-mapper-test -t json-ser-full)
  add_test(json-deser-list ${EXECUTABLE_OUTPUT_PATH}/anch-json-mapper-test -t json-deser-list)
  add_test(json-deser-unf-field ${EXECUTABLE_OUTPUT_PATH}/anch-json-mapper-test -t json-deser-unf-field)
  add_test(json-deser-unf-str ${EXECUTABLE_OUTPUT_PATH}/anch-json-mapper-test -t json-deser-unf-str)
  add_test(json-deser-unf ${EXECUTABLE_OUTPUT_PATH}/anch-json-mapper-test -t json-deser-unf)
  add_test(json-deser-unf-int ${EXECUTABLE_OUTPUT_PATH}/anch-json-mapper-test -t json-deser-unf-int)
  add_test(json-deser-unf-empty-col ${EXECUTABLE_OUTPUT_PATH}/anch-json-mapper-test -t json-deser-unf-empty-col)
  add_test(json-deser-unf-int-col ${EXECUTABLE_OUTPUT_PATH}/anch-json-mapper-test -t json-deser-unf-int-col)
  add_test(json-deser-unf-wtf-col ${EXECUTABLE_OUTPUT_PATH}/anch-json-mapper-test -t json-deser-unf-wtf-col)
  add_test(json-full-deser-default ${EXECUTABLE_OUTPUT_PATH}/anch-json-mapper-test -t json-full-deser-default)
  add_test(json-deser-discard128 ${EXECUTABLE_OUTPUT_PATH}/anch-json-mapper-test -t json-deser-discard128)
  add_test(json-deser-discard128-col ${EXECUTABLE_OUTPUT_PATH}/anch-json-mapper-test -t json-deser-discard128-col)
  add_test(json-iomanip ${EXECUTABLE_OUTPUT_PATH}/anch-json-mapper-test -t json-iomanip)
  add_test(json-ser-map ${EXECUTABLE_OUTPUT_PATH}/anch-json-mapper-test -t json-ser-map)
  add_test(json-deser-map ${EXECUTABLE_OUTPUT_PATH}/anch-json-mapper-test -t json-deser-map)
  # Mapper unit tests -

endif(ANCH_JSON_UNITARY_TEST)
