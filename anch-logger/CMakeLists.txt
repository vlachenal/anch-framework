cmake_minimum_required(VERSION 3.10)

# Project anch logger
# ANother C++ Hack
project(anch-logger)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include(AnchLogger.cmake.in)

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
add_definitions(${ANCH_LOGGER_COMPILE_FLAGS})

# Includes location
include_directories(
  inc
  ${ANCH_RESOURCE_INCLUDES_DIR}
  ${ANCH_DATE_INCLUDES_DIR}
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
  anch-logger-obj
  OBJECT
  ${source_files}
  )

## Dynamic library +
add_library(
  anch-logger
  SHARED
  $<TARGET_OBJECTS:anch-logger-obj>
  )

install(
  DIRECTORY inc/ DESTINATION ${ANCH_LOGGER_INSTALL_INC_DIR}
  FILES_MATCHING PATTERN "*.hpp"
  )

install(
  TARGETS anch-logger
  LIBRARY DESTINATION ${ANCH_LOGGER_INSTALL_LIB_DIR}
  )
## Dynamic library -

## Static library +
if(ANCH_LOGGER_STATIC_LIB)
  add_library(
    anch-logger-static
    STATIC
    $<TARGET_OBJECTS:anch-logger-obj>
    )
  set_target_properties(anch-logger-static PROPERTIES OUTPUT_NAME anch-logger)

  install(
    TARGETS anch-logger-static
    ARCHIVE DESTINATION ${ANCH_LOGGER_INSTALL_LIB_DIR}
    )
endif(ANCH_LOGGER_STATIC_LIB)
## Static library -


# Unitary tests
if(ANCH_LOGGER_UNITARY_TEST)

  link_directories(
    ${ANCH_RESOURCE_LIBRARY_DIR}
    ${ANCH_DATE_LIBRARY_DIR}
    )

  # build rotate test +
  file(
    GLOB_RECURSE
    test_rotate
    test/testRotate.cpp
  )

  add_executable(
    anch-logger-rotate
    ${test_rotate}
  )

  target_link_libraries(
    anch-logger-rotate
    anch-logger
    anch-resource
    anch-date
    )

  configure_file(test/anch-logger.conf anch-logger.conf COPYONLY)
  # build rotate test -

  # build custom file test +
  file(
    GLOB_RECURSE
    test_custom_file
    test/testCustomFile.cpp
  )

  add_executable(
    anch-logger-custom-file
    ${test_custom_file}
  )

  list(APPEND custom_file_def "_ANCH_LOGGER_CONFIG_FILE_=\"customFile.ini\"")
  set_target_properties(
    anch-logger-custom-file
    PROPERTIES
    COMPILE_DEFINITIONS "${custom_file_def}"
  )

  target_link_libraries(
    anch-logger-custom-file
    anch-logger
    anch-resource
    anch-date
    )

  configure_file(test/customFile.ini customFile.ini COPYONLY)
  # build custom file test -

  # build low priority file test +
  file(
    GLOB_RECURSE
    test_low_priority_file
    test/testLowPriority.cpp
  )

  add_executable(
    anch-logger-low-priority
    ${test_low_priority_file}
  )

  list(APPEND low_priority_file_def "_ANCH_LOGGER_CONFIG_FILE_=\"low-priority-logger.conf\"")
  set_target_properties(
    anch-logger-low-priority
    PROPERTIES
    COMPILE_DEFINITIONS "${low_priority_file_def}"
  )

  target_link_libraries(
    anch-logger-low-priority
    anch-logger
    anch-resource
    anch-date
    )

  configure_file(test/low-priority-logger.conf low-priority-logger.conf COPYONLY)
  # build low priority file test -

  # build default configuration test +
  file(
    GLOB_RECURSE
    test_default_conf
    inc/*
    test/testDefaultConfig.cpp
  )

  add_executable(
    anch-logger-default-config
    ${test_default_conf}
  )

  list(APPEND default_conf_def "_ANCH_LOGGER_CONFIG_FILE_=\"invalid.conf\"")
  set_target_properties(
    anch-logger-default-config
    PROPERTIES
    COMPILE_DEFINITIONS "${default_conf_def}"
  )

  target_link_libraries(
    anch-logger-default-config
    anch-logger
    anch-resource
    anch-date
    )
  # build default configuration test -

  enable_testing()
  add_test(anch-logger-rotate ${EXECUTABLE_OUTPUT_PATH}/anch-logger-rotate main)
  add_test(anch-logger-custom-file ${EXECUTABLE_OUTPUT_PATH}/anch-logger-custom-file main)
  add_test(anch-logger-low-priority ${EXECUTABLE_OUTPUT_PATH}/anch-logger-low-priority main)
  add_test(anch-logger-default-config ${EXECUTABLE_OUTPUT_PATH}/anch-logger-default-config main)

endif(ANCH_LOGGER_UNITARY_TEST)
