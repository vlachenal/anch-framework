cmake_minimum_required(VERSION 2.6)

# Project anch crypto
# ANother C++ Harsh/Huge/Hack
project(anch-crypto)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include(AnchCrypto.cmake.in)

# Check GCC version +
execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                OUTPUT_VARIABLE GCC_VERSION)
if (GCC_VERSION VERSION_LESS 4.5)
  message(FATAL_ERROR "Version < 4.5. This library is based on C++11 standard. Exit.")
elseif(GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7)
  message(STATUS "Version >= 4.7 use -std=c++11")
  add_definitions("-std=c++11")
else()
  message(STATUS "4.5 <= Version < 4.7 use -std=c++0x")
  add_definitions("-std=c++0x")
endif()
# Check GCC version -

# Add GCC definitions
add_definitions("-Wall -Werror")
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  add_definitions("-D ANCH_WINDOWS")
else()
  add_definitions("-D ANCH_POSIX")
endif()
add_definitions(${ANCH_CRYPTO_COMPILE_FLAGS})

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
  inc/*
)

# Declare exec
add_library(
  anch-crypto
  SHARED
  ${source_files}
)

# Unitary tests
if(ANCH_CRYPTO_UNITARY_TEST)

  # Build MD5 unitary test +
  file(
    GLOB_RECURSE
    test_md5
    inc/*
    test/testMD5.cpp
  )

  add_executable(
    anch-md5-test
    ${test_md5}
  )

  target_link_libraries(
    anch-md5-test
    anch-crypto
  )
  # Build MD5 unitary test -

  # Build SHA1 unitary test +
  file(
    GLOB_RECURSE
    test_sha1
    ${ANCH_UTILS_INCLUDES_DIR}/*
    inc/*
    test/testSHA1.cpp
  )

  add_executable(
    anch-sha1-test
    ${test_sha1}
  )

  target_link_libraries(
    anch-sha1-test
    anch-crypto
  )
  # Build SHA1 unitary test -

  # Build SHA2 unitary test +
  file(
    GLOB_RECURSE
    test_sha2
    ${ANCH_UTILS_INCLUDES_DIR}/*
    inc/*
    test/testSHA2.cpp
  )

  add_executable(
    anch-sha2-test
    ${test_sha2}
  )

  target_link_libraries(
    anch-sha2-test
    anch-crypto
  )
  # Build SHA2 unitary test -

  # Build HMAC unitary test +
  file(
    GLOB_RECURSE
    test_hmac
    inc/*
    test/testHmac.cpp
  )

  add_executable(
    anch-hmac-test
    ${test_hmac}
  )

  target_link_libraries(
    anch-hmac-test
    anch-crypto
  )
  # Build HMAC unitary test -

  # Build Base64 unitary test +
  file(
    GLOB_RECURSE
    test_base64
    inc/*
    test/testBase64.cpp
  )

  add_executable(
    anch-base64-test
    ${test_base64}
  )

  target_link_libraries(
    anch-base64-test
    anch-crypto
  )
  # Build Base64 unitary test -

  # Build AES unitary test +
  file(
    GLOB_RECURSE
    test_aes
    inc/*
    test/testAES.cpp
  )

  add_executable(
    anch-aes-test
    ${test_aes}
  )

  target_link_libraries(
    anch-aes-test
    anch-crypto
  )
  # Build AES unitary test -

  enable_testing()
  add_test(anch-md5-test ${EXECUTABLE_OUTPUT_PATH}/anch-md5-test main)
  add_test(anch-sha1-test ${EXECUTABLE_OUTPUT_PATH}/anch-sha1-test main)
  add_test(anch-sha2-test ${EXECUTABLE_OUTPUT_PATH}/anch-sha2-test main)
  add_test(anch-hmac-test ${EXECUTABLE_OUTPUT_PATH}/anch-hmac-test main)
  add_test(anch-base64-test ${EXECUTABLE_OUTPUT_PATH}/anch-base64-test main)
  add_test(anch-aes-test ${EXECUTABLE_OUTPUT_PATH}/anch-aes-test main)

endif(ANCH_CRYPTO_UNITARY_TEST)
