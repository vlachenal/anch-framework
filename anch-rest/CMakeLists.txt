cmake_minimum_required(VERSION 3.10)

# Project anch rest
# ANother C++ Harsh/Huge/Hack
project(anch-rest)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include(AnchRest.cmake.in)

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
add_definitions(${ANCH_REST_COMPILE_FLAGS})

# Include librairies
include(FindThreads)
find_package(Threads)

## Common library +
# Includes location
include_directories(
  common/inc
  ${ANCH_UTILS_INCLUDES_DIR}
  ${ANCH_JSON_INCLUDES_DIR}
  )

# Retrieve every files in project
file(
  GLOB_RECURSE
  source_files
  common/src/*
  )

# Build objects
add_library(
  anch-rest-common-obj
  OBJECT
  ${source_files}
  )

# Dynamic library +
add_library(
  anch-rest-common
  SHARED
  $<TARGET_OBJECTS:anch-rest-common-obj>
  )

install(
  DIRECTORY inc/ DESTINATION ${ANCH_REST_INSTALL_INC_DIR}
  FILES_MATCHING PATTERN "*.hpp"
  )

install(
  TARGETS anch-rest-common
  LIBRARY DESTINATION ${ANCH_REST_INSTALL_LIB_DIR}
  )
# Dynamic library -

# Static library +
if(ANCH_REST_STATIC_LIB)
  add_library(
    anch-rest-common-static
    STATIC
    $<TARGET_OBJECTS:anch-rest-common-obj>
    )
  set_target_properties(anch-rest-common-static PROPERTIES OUTPUT_NAME anch-rest-common)

  install(
    TARGETS anch-rest-common-static
    ARCHIVE DESTINATION ${ANCH_REST_INSTALL_LIB_DIR}
    )
endif(ANCH_REST_STATIC_LIB)
# Static library -
## Common library -


if(ANCH_REST_SERVER_LIB)
  ## Router library +
  # Includes location
  include_directories(
    router/inc
    common/inc
    ${ANCH_UUID_INCLUDES_DIR}
    ${ANCH_UTILS_INCLUDES_DIR}
    )

  # Retrieve every files in project
  file(
    GLOB_RECURSE
    source_files
    router/src/*
    )

  # Build objects
  add_library(
    anch-rest-router-obj
    OBJECT
    ${source_files}
    )

  # Dynamic library +
  add_library(
    anch-rest-router
    SHARED
    $<TARGET_OBJECTS:anch-rest-router-obj>
    )

  install(
    DIRECTORY inc/ DESTINATION ${ANCH_REST_INSTALL_INC_DIR}
    FILES_MATCHING PATTERN "*.hpp"
    )

  install(
    TARGETS anch-rest-router
    LIBRARY DESTINATION ${ANCH_REST_INSTALL_LIB_DIR}
    )
  # Dynamic library -

  # Static library +
  if(ANCH_REST_STATIC_LIB)
    add_library(
      anch-rest-router-static
      STATIC
      $<TARGET_OBJECTS:anch-rest-router-obj>
      )
    set_target_properties(anch-rest-router-static PROPERTIES OUTPUT_NAME anch-rest-router)

    install(
      TARGETS anch-rest-router-static
      ARCHIVE DESTINATION ${ANCH_REST_INSTALL_LIB_DIR}
      )
  endif(ANCH_REST_STATIC_LIB)
  # Static library -
  ## Router library -
endif(ANCH_REST_SERVER_LIB)

if(ANCH_REST_CLIENT_LIB)
  ## Client library +
  # Includes location
  include_directories(
    client/inc
    common/inc
    ${ANCH_CRYPTO_INCLUDES_DIR}
    ${ANCH_UTILS_INCLUDES_DIR}
    )

  # Retrieve every files in project
  file(
    GLOB_RECURSE
    source_files
    client/src/*
    )

  # Build objects
  add_library(
    anch-rest-client-obj
    OBJECT
    ${source_files}
    )

  # Dynamic library +
  add_library(
    anch-rest-client
    SHARED
    $<TARGET_OBJECTS:anch-rest-client-obj>
    )

  install(
    DIRECTORY inc/ DESTINATION ${ANCH_REST_INSTALL_INC_DIR}
    FILES_MATCHING PATTERN "*.hpp"
    )

  install(
    TARGETS anch-rest-client
    LIBRARY DESTINATION ${ANCH_REST_INSTALL_LIB_DIR}
    )
  # Dynamic library -

  # Static library +
  if(ANCH_REST_STATIC_LIB)
    add_library(
      anch-rest-client-static
      STATIC
      $<TARGET_OBJECTS:anch-rest-client-obj>
      )
    set_target_properties(anch-rest-client-static PROPERTIES OUTPUT_NAME anch-rest-client)

    install(
      TARGETS anch-rest-client-static
      ARCHIVE DESTINATION ${ANCH_REST_INSTALL_LIB_DIR}
      )
  endif(ANCH_REST_STATIC_LIB)
  # Static library -
  ## Client library -

  ## CURL implementation library +
  if(ANCH_REST_CURL_LIB)
    # Include librairies
    include(FindCURL)
    find_package(CURL)

  endif(ANCH_REST_CURL_LIB)
  ## CURL implementation library -
endif(ANCH_REST_CLIENT_LIB)

# Unitary tests
if(ANCH_REST_UNITARY_TEST)

  link_directories(
    ${ANCH_UUID_LIBRARY_DIR}
    ${ANCH_JSON_LIBRARY_DIR}
    )

  if(ANCH_REST_SERVER_LIB)
    # Build router unit tests +
    file(
      GLOB_RECURSE
      test_router
      test/testRouter.cpp
      )

    add_executable(
      anch-router-test
      ${test_router}
      )

    target_link_libraries(
      anch-router-test
      anch-rest-common
      anch-rest-router
      anch-uuid
      anch-json
      ${CMAKE_THREAD_LIBS_INIT}
      )
    # Build router unit tests -

    file(
      GLOB_RECURSE
      test_mapper_reg
      test/testMapperRegistry.cpp
      )

    add_executable(
      anch-mapper-registry-test
      ${test_mapper_reg}
      )

    target_link_libraries(
      anch-mapper-registry-test
      anch-rest-common
      anch-uuid
      anch-json
      ${CMAKE_THREAD_LIBS_INIT}
      )
  endif(ANCH_REST_SERVER_LIB)

  enable_testing()

  add_test(mapper-JSON ${EXECUTABLE_OUTPUT_PATH}/anch-mapper-registry-test plop)

  if(ANCH_REST_SERVER_LIB)
    add_test(router-GET-no-param-no-body-OK ${EXECUTABLE_OUTPUT_PATH}/anch-router-test test_no_param_no_body_ok)
    add_test(router-GET-1-param-no-body-OK ${EXECUTABLE_OUTPUT_PATH}/anch-router-test test_1_param_no_body_ok)
    add_test(router-GET-1-int-param-no-body-OK ${EXECUTABLE_OUTPUT_PATH}/anch-router-test test_1_int_param_no_body_ok)
    add_test(router-GET-1-int-param-no-body-KO ${EXECUTABLE_OUTPUT_PATH}/anch-router-test test_1_int_param_no_body_ko)
    add_test(router-GET-1-view-param-no-body-OK ${EXECUTABLE_OUTPUT_PATH}/anch-router-test test_1_view_param_no_body_ok)
    add_test(router-POST-error-405 ${EXECUTABLE_OUTPUT_PATH}/anch-router-test test_error_405)
    add_test(router-GET-error-406 ${EXECUTABLE_OUTPUT_PATH}/anch-router-test test_error_406)
    add_test(router-GET-error-415 ${EXECUTABLE_OUTPUT_PATH}/anch-router-test test_error_415)
    add_test(router-GET-error-404 ${EXECUTABLE_OUTPUT_PATH}/anch-router-test test_error_404)
    add_test(router-GET-uuid-ok ${EXECUTABLE_OUTPUT_PATH}/anch-router-test test_uuid_ok)
    add_test(router-GET-uuid-400 ${EXECUTABLE_OUTPUT_PATH}/anch-router-test test_uuid_ko)
    add_test(router-GET-multi-parameters-ok ${EXECUTABLE_OUTPUT_PATH}/anch-router-test test_multi_ok)
    add_test(router-GET-lambda-ok ${EXECUTABLE_OUTPUT_PATH}/anch-router-test test_lambda_ok)
    add_test(router-GET-json-ok ${EXECUTABLE_OUTPUT_PATH}/anch-router-test test_res_json_ok)
    add_test(router-GET-istream-ok ${EXECUTABLE_OUTPUT_PATH}/anch-router-test test_res_istream_ok)
  endif(ANCH_REST_SERVER_LIB)

  if(ANCH_REST_CLIENT_LIB)
    if(ANCH_REST_CURL_LIB)
    endif(ANCH_REST_CURL_LIB)
  endif(ANCH_REST_CLIENT_LIB)

endif(ANCH_REST_UNITARY_TEST)
