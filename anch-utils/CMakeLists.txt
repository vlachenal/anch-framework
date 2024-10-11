cmake_minimum_required(VERSION 3.10)

# Project anch utils
# ANother C++ Harsh/Huge/Hack
project(anch-utils)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include(AnchUtils.cmake.in)

# Check GCC version +
execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                OUTPUT_VARIABLE GCC_VERSION)
if (GCC_VERSION VERSION_LESS 8.0)
  message(FATAL_ERROR "Version < 8.0. This library is based on C++17 standard. Exit.")
elseif(GCC_VERSION VERSION_GREATER 10.0 OR GCC_VERSION VERSION_EQUAL 10.0)
  message(STATUS "Version >= 10.0 use -std=c++20")
  add_definitions("-std=c++20")
  add_definitions("-D ANCH_STD_OTP")
else()
  message(STATUS "Version >= 8.0 use -std=c++17")
  add_definitions("-std=c++17")
  add_definitions("-D ANCH_STD_OTP")
endif()
# Check GCC version -

# Add GCC definitions
add_definitions("-Wall -Wextra -Werror -Wuninitialized -Wconversion -Wsign-conversion -Wformat-security -fstack-protector-all -Wstack-protector --param ssp-buffer-size=4 -ftrapv -z relro,now -fPIC -D_FORTIFY_SOURCE=2")
#add_definitions("-Wall -Wextra -Werror -Wuninitialized -Wconversion -Wsign-conversion -Wformat-security -fstack-protector-all -Wstack-protector --param ssp-buffer-size=4 -ftrapv -z relro,now -fPIC")
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  add_definitions("-D ANCH_WINDOWS")
else()
  add_definitions("-D ANCH_POSIX")
endif()
add_definitions(${ANCH_UTILS_COMPILE_FLAGS})

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

install(
  DIRECTORY inc/ DESTINATION ${ANCH_UTILS_INSTALL_INC_DIR}
  FILES_MATCHING PATTERN "*.hpp"
  )

# Unit tests
if(ANCH_UTILS_UNIT_TEST)

  # Includes location
  include_directories(
    inc
    ${ANCH_UT_INCLUDES_DIR}
    )

  link_directories(
    ${ANCH_UT_LIBRARY_DIR}
    ${ANCH_CLI_LIBRARY_DIR}
    )

  # Build thread pool unit test +
  file(
    GLOB_RECURSE
    test_thread_pool
    test/testThreadPool.cpp
  )

  add_executable(
    anch-thread-pool-test
    ${test_thread_pool}
  )

  target_link_libraries(
    anch-thread-pool-test
    anch-unit
    anch-cli
  )
  # Build thread pool unit test -

  # Build resource pool unit test +
  file(
    GLOB_RECURSE
    test_res_pool
    test/testResourcedPool.cpp
  )

  add_executable(
    anch-res-pool-test
    ${test_res_pool}
  )

  target_link_libraries(
    anch-res-pool-test
    anch-unit
    anch-cli
  )
  # Build resource pool unit test -

  # Build stream unit test +
  file(
    GLOB_RECURSE
    test_stream
    test/testStream.cpp
  )

  add_executable(
    anch-stream-test
    ${test_stream}
  )

  target_link_libraries(
    anch-stream-test
  )
  # Build stream unit test -

  # Build convert test +
  file(
    GLOB_RECURSE
    test_convert
    test/testConvert.cpp
  )

  add_executable(
    anch-convert-test
    ${test_convert}
  )

  target_link_libraries(
    anch-convert-test
    anch-unit
    anch-cli
  )
  # Build convert test -

  # Build string utils test +
  file(
    GLOB_RECURSE
    test_string_utils
    test/testStringUtils.cpp
  )

  add_executable(
    anch-string-utils-test
    ${test_string_utils}
  )
  # Build string utils test -

  enable_testing()
  # Thread pool +
  add_test(anch-thread-pool-sleep2s-test ${EXECUTABLE_OUTPUT_PATH}/anch-thread-pool-test -t thread-pool-sleep2s)
  add_test(anch-thread-pool-class-test ${EXECUTABLE_OUTPUT_PATH}/anch-thread-pool-test -t thread-pool-class)
  # Thread pool -

  # Resource pool +
  add_test(anch-res-pool1-test ${EXECUTABLE_OUTPUT_PATH}/anch-res-pool-test -t res-pool1)
  add_test(anch-res-pool2-test ${EXECUTABLE_OUTPUT_PATH}/anch-res-pool-test -t res-pool2)
  # Resource pool -

  # Fluent +
  add_test(anch-fluent-foreach ${EXECUTABLE_OUTPUT_PATH}/anch-stream-test foreach)
  add_test(anch-fluent-filter ${EXECUTABLE_OUTPUT_PATH}/anch-stream-test filter)
  add_test(anch-fluent-skip ${EXECUTABLE_OUTPUT_PATH}/anch-stream-test skip)
  add_test(anch-fluent-limit ${EXECUTABLE_OUTPUT_PATH}/anch-stream-test limit)
  add_test(anch-fluent-concat ${EXECUTABLE_OUTPUT_PATH}/anch-stream-test concat)
  add_test(anch-fluent-all-match ${EXECUTABLE_OUTPUT_PATH}/anch-stream-test allMatch)
  add_test(anch-fluent-any-match ${EXECUTABLE_OUTPUT_PATH}/anch-stream-test anyMatch)
  add_test(anch-fluent-any-match ${EXECUTABLE_OUTPUT_PATH}/anch-stream-test noneMatch)
  add_test(anch-fluent-map ${EXECUTABLE_OUTPUT_PATH}/anch-stream-test map)
  add_test(anch-fluent-collect-list ${EXECUTABLE_OUTPUT_PATH}/anch-stream-test collect_list)
  add_test(anch-fluent-collect-forward-list ${EXECUTABLE_OUTPUT_PATH}/anch-stream-test collect_forward_list)
  add_test(anch-fluent-collect-set ${EXECUTABLE_OUTPUT_PATH}/anch-stream-test collect_set)
  add_test(anch-fluent-collect-vector ${EXECUTABLE_OUTPUT_PATH}/anch-stream-test collect_vector)
  add_test(anch-fluent-collect-multiset ${EXECUTABLE_OUTPUT_PATH}/anch-stream-test collect_multiset)
  add_test(anch-fluent-collect-unordered-set ${EXECUTABLE_OUTPUT_PATH}/anch-stream-test collect_unordered_set)
  add_test(anch-fluent-collect-unordered-multiset ${EXECUTABLE_OUTPUT_PATH}/anch-stream-test collect_unordered_multiset)
  add_test(anch-fluent-collect-deque ${EXECUTABLE_OUTPUT_PATH}/anch-stream-test collect_deque)
  add_test(anch-fluent-collect-stack ${EXECUTABLE_OUTPUT_PATH}/anch-stream-test collect_stack)
  add_test(anch-fluent-collect-queue ${EXECUTABLE_OUTPUT_PATH}/anch-stream-test collect_queue)
  add_test(anch-fluent-collect-priority-queue ${EXECUTABLE_OUTPUT_PATH}/anch-stream-test collect_priority_queue)
  # Fluent -

  # Convert +
  add_test(anch-convert--32-as-short ${EXECUTABLE_OUTPUT_PATH}/anch-convert-test -t conv--32-as-short)
  add_test(anch-convert--32-as-ushort ${EXECUTABLE_OUTPUT_PATH}/anch-convert-test -t conv--32-as-ushort)
  add_test(anch-convert--32.2-as-ushort ${EXECUTABLE_OUTPUT_PATH}/anch-convert-test -t conv--32.2-as-ushort)
  add_test(anch-convert-32-as-short ${EXECUTABLE_OUTPUT_PATH}/anch-convert-test -t conv-32-as-short)
  add_test(anch-convert-32-as-ushort ${EXECUTABLE_OUTPUT_PATH}/anch-convert-test -t conv-32-as-ushort)
  add_test(anch-convert-32.2-as-short ${EXECUTABLE_OUTPUT_PATH}/anch-convert-test -t conv-32.2-as-short)
  add_test(anch-convert-32e-1-as-short ${EXECUTABLE_OUTPUT_PATH}/anch-convert-test -t conv-32e-1-as-short)
  add_test(anch-convert-32e-1-as-ushort ${EXECUTABLE_OUTPUT_PATH}/anch-convert-test -t conv-32e-1-as-ushort)
  # Convert -

  # String +
  add_test(anch-string-utils ${EXECUTABLE_OUTPUT_PATH}/anch-string-utils-test main)
  # String -

  # Build optional unit test +
  file(
    GLOB_RECURSE
    test_optional
    test/testOptional.cpp
    )

  add_executable(
    anch-optional-test
    ${test_optional}
    )

  target_link_libraries(
    anch-optional-test
    )
  # Build optional unit test -

  add_test(anch-optional-test ${EXECUTABLE_OUTPUT_PATH}/anch-optional-test main)

endif(ANCH_UTILS_UNIT_TEST)
