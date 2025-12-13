cmake_minimum_required(VERSION 3.10)

# Project anch conf
# ANother C++ Harsh/Huge/Hack
project(anch-conf)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include(AnchConf.cmake.in)

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
add_definitions("-Wall -Wextra -Werror -Wuninitialized -Wconversion -Wsign-conversion -Wformat-security -fstack-protector-all -Wstack-protector --param ssp-buffer-size=4 -ftrapv  -z relro,now -fPIC -D_FORTIFY_SOURCE=2")
add_definitions(${ANCH_CONF_COMPILE_FLAGS})

# Includes location
include_directories(
  inc
  ${ANCH_INI_INCLUDES_DIR}
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
  anch-conf-obj
  OBJECT
  ${source_files}
  )

## Dynamic library +
add_library(
  anch-conf
  SHARED
  $<TARGET_OBJECTS:anch-conf-obj>
  )

install(
  DIRECTORY inc/ DESTINATION ${ANCH_CONF_INSTALL_INC_DIR}
  FILES_MATCHING PATTERN "*.hpp"
  )

install(
  TARGETS anch-conf
  LIBRARY DESTINATION ${ANCH_CONF_INSTALL_LIB_DIR}
  )
## Dynamic library -

## Static library +
if(ANCH_CONF_STATIC_LIB)
  add_library(
    anch-conf-static
    STATIC
    $<TARGET_OBJECTS:anch-conf-obj>
    )
  set_target_properties(anch-conf-static PROPERTIES OUTPUT_NAME anch-conf)

  install(
    TARGETS anch-conf-static
    ARCHIVE DESTINATION ${ANCH_CONF_INSTALL_LIB_DIR}
    )
endif(ANCH_CONF_STATIC_LIB)
## Static library -


if(ANCH_CONF_UNITARY_TEST)

  include_directories(
    inc
    ${ANCH_INI_INCLUDES_DIR}
    ${ANCH_CLI_INCLUDES_DIR}
    ${ANCH_UT_INCLUDES_DIR}
    )

  link_directories(
    ${ANCH_CLI_LIBRARY_DIR}
    ${ANCH_UT_LIBRARY_DIR}
    ${ANCH_INI_LIBRARY_DIR}
    )

  configure_file(test/test.ini test.ini COPYONLY)
  configure_file(test/test-UT.conf test-UT.conf COPYONLY)
  configure_file(test/test-defprof.ini test-defprof.ini COPYONLY)
  configure_file(test/test-defprof-UT.conf test-defprof-UT.conf COPYONLY)
  # Build parser unitary test +
  file(
    GLOB_RECURSE
    test_files
    test/testConf.cpp
    )

  add_executable(
    anch-conf-parse
    ${test_files}
    )

  target_link_libraries(
    anch-conf-parse
    anch-conf
    anch-ini
    anch-cli
    anch-unit
    )
  # Build parser unitary test -

  # Build parser unitary test +
  file(
    GLOB_RECURSE
    test_profile_files
    test/testConfProfile.cpp
    )

  add_executable(
    anch-conf-profile-parse
    ${test_profile_files}
    )

  target_link_libraries(
    anch-conf-profile-parse
    anch-conf
    anch-ini
    anch-cli
    anch-unit
    )
  # Build parser unitary test -

  # Build parser unitary test +
  file(
    GLOB_RECURSE
    test_default_profile_files
    test/testDefaultProfiles.cpp
    )

  add_executable(
    anch-conf-default-profile-parse
    ${test_default_profile_files}
    )

  target_link_libraries(
    anch-conf-default-profile-parse
    anch-conf
    anch-ini
    anch-cli
    anch-unit
    )
  # Build parser unitary test -

  enable_testing()
  add_test(anch-conf-get-TOTO-toto ${EXECUTABLE_OUTPUT_PATH}/anch-conf-parse -t get-TOTO-toto)
  add_test(anch-conf-get-tata ${EXECUTABLE_OUTPUT_PATH}/anch-conf-parse -t get-tata)
  add_test(anch-conf-get-toto ${EXECUTABLE_OUTPUT_PATH}/anch-conf-parse -t get-toto)

  add_test(anch-conf-prof-get-TOTO-toto ${EXECUTABLE_OUTPUT_PATH}/anch-conf-profile-parse -t get-TOTO-toto)
  add_test(anch-conf-prof-get-tata ${EXECUTABLE_OUTPUT_PATH}/anch-conf-profile-parse -t get-tata)
  add_test(anch-conf-prof-get-toto ${EXECUTABLE_OUTPUT_PATH}/anch-conf-profile-parse -t get-toto)

  add_test(anch-conf-defprof-get-TOTO-toto ${EXECUTABLE_OUTPUT_PATH}/anch-conf-default-profile-parse -t get-TOTO-toto)
  add_test(anch-conf-defprof-get-tata ${EXECUTABLE_OUTPUT_PATH}/anch-conf-default-profile-parse -t get-tata)
  add_test(anch-conf-defprof-get-toto ${EXECUTABLE_OUTPUT_PATH}/anch-conf-default-profile-parse -t get-toto)

endif(ANCH_CONF_UNITARY_TEST)
