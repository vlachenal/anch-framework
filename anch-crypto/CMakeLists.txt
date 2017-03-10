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
elseif(GCC_VERSION VERSION_GREATER 6.0 OR GCC_VERSION VERSION_EQUAL 6.0)
  message(STATUS "Version >= 6.0 use -std=c++14")
  add_definitions("-std=c++14")
elseif(GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7)
  message(STATUS "Version >= 4.7 use -std=c++11")
  add_definitions("-std=c++11")
else()
  message(STATUS "4.5 <= Version < 4.7 use -std=c++0x")
  add_definitions("-std=c++0x")
endif()
# Check GCC version -

# Add GCC definitions
add_definitions("-Wall -Wextra -Werror -Wuninitialized -Wconversion -Wsign-conversion -Wformat-security -fstack-protector-all -Wstack-protector --param ssp-buffer-size=4 -ftrapv -D_FORTIFY_SOURCE=2 -z relro,now -fPIC")
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  add_definitions("-D ANCH_WINDOWS")
else()
  add_definitions("-D ANCH_POSIX")
endif()
add_definitions(${ANCH_CRYPTO_COMPILE_FLAGS})

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
    ${CMAKE_THREAD_LIBS_INIT}
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
    ${CMAKE_THREAD_LIBS_INIT}
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
    ${CMAKE_THREAD_LIBS_INIT}
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
    ${CMAKE_THREAD_LIBS_INIT}
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
    ${CMAKE_THREAD_LIBS_INIT}
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
    ${CMAKE_THREAD_LIBS_INIT}
  )
  # Build AES unitary test -

  # Build padding unitary test +
  file(
    GLOB_RECURSE
    test_padding
    inc/*
    test/testPadding.cpp
  )

  add_executable(
    anch-padding-test
    ${test_padding}
  )

  target_link_libraries(
    anch-padding-test
    anch-crypto
    ${CMAKE_THREAD_LIBS_INIT}
  )
  # Build padding unitary test -

  # Build AES - ECB unit tests +
  file(
    GLOB_RECURSE
    test_aes_ecb
    inc/*
    test/testAesEcb.cpp
  )

  add_executable(
    anch-aes-ecb-test
    ${test_aes_ecb}
  )

  target_link_libraries(
    anch-aes-ecb-test
    anch-crypto
    ${CMAKE_THREAD_LIBS_INIT}
  )
  # Build AES-ECB unit tests -

  # Build AES - CBC unit tests +
  file(
    GLOB_RECURSE
    test_aes_cbc
    inc/*
    test/testAesCbc.cpp
  )

  add_executable(
    anch-aes-cbc-test
    ${test_aes_cbc}
  )

  target_link_libraries(
    anch-aes-cbc-test
    anch-crypto
    ${CMAKE_THREAD_LIBS_INIT}
  )
  # Build AES-CBC unit tests -

  # Build AES - PCBC unit tests +
  file(
    GLOB_RECURSE
    test_aes_pcbc
    inc/*
    test/testAesPcbc.cpp
  )

  add_executable(
    anch-aes-pcbc-test
    ${test_aes_pcbc}
  )

  target_link_libraries(
    anch-aes-pcbc-test
    anch-crypto
    ${CMAKE_THREAD_LIBS_INIT}
  )
  # Build AES-PCBC unit tests -

  # Build AES - CTR unit tests +
  file(
    GLOB_RECURSE
    test_aes_ctr
    inc/*
    test/testAesCtr.cpp
  )

  add_executable(
    anch-aes-ctr-test
    ${test_aes_ctr}
  )

  target_link_libraries(
    anch-aes-ctr-test
    anch-crypto
    ${CMAKE_THREAD_LIBS_INIT}
  )
  # Build AES-CTR unit tests -

  # Build AES - OFB unit tests +
  file(
    GLOB_RECURSE
    test_aes_ofb
    inc/*
    test/testAesOfb.cpp
  )

  add_executable(
    anch-aes-ofb-test
    ${test_aes_ofb}
  )

  target_link_libraries(
    anch-aes-ofb-test
    anch-crypto
    ${CMAKE_THREAD_LIBS_INIT}
  )
  # Build AES-OFB unit tests -

  # Build AES - CFB unit tests +
  file(
    GLOB_RECURSE
    test_aes_cfb
    inc/*
    test/testAesCfb.cpp
  )

  add_executable(
    anch-aes-cfb-test
    ${test_aes_cfb}
  )

  target_link_libraries(
    anch-aes-cfb-test
    anch-crypto
    ${CMAKE_THREAD_LIBS_INIT}
  )
  # Build AES-CFB unit tests -

  enable_testing()
  add_test(MD5 ${EXECUTABLE_OUTPUT_PATH}/anch-md5-test main)
  add_test(SHA1 ${EXECUTABLE_OUTPUT_PATH}/anch-sha1-test main)

  add_test(SHA2-224 ${EXECUTABLE_OUTPUT_PATH}/anch-sha2-test 224)
  add_test(SHA2-256 ${EXECUTABLE_OUTPUT_PATH}/anch-sha2-test 256)
  add_test(SHA2-384 ${EXECUTABLE_OUTPUT_PATH}/anch-sha2-test 384)
  add_test(SHA2-512 ${EXECUTABLE_OUTPUT_PATH}/anch-sha2-test 512)

  add_test(HMAC ${EXECUTABLE_OUTPUT_PATH}/anch-hmac-test main)
  add_test(Base64 ${EXECUTABLE_OUTPUT_PATH}/anch-base64-test main)

  add_test(AES128 ${EXECUTABLE_OUTPUT_PATH}/anch-aes-test 128)
  add_test(AES192 ${EXECUTABLE_OUTPUT_PATH}/anch-aes-test 192)
  add_test(AES256 ${EXECUTABLE_OUTPUT_PATH}/anch-aes-test 256)

  add_test(Padding ${EXECUTABLE_OUTPUT_PATH}/anch-padding-test main)

  add_test(AesEcb-ISO7816_4-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test ISO7816_4 sequential)
  add_test(AesEcb-ISO7816_4-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test ISO7816_4 parallel)
  add_test(AesEcb-PKCS7-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test "PKCS#7" sequential)
  add_test(AesEcb-PKCS7-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test "PKCS#7" parallel)
  add_test(AesEcb-zero-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test zero sequential)
  add_test(AesEcb-zero-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test zero parallel)
  add_test(AesEcb-ANSIX923-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test ANSIX923 sequential)
  add_test(AesEcb-ANSIX923-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test ANSIX923 parallel)

  add_test(AesCbc-ISO7816_4-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test ISO7816_4 sequential)
  add_test(AesCbc-ISO7816_4-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test ISO7816_4 parallel)
  add_test(AesCbc-PKCS7-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test "PKCS#7" sequential)
  add_test(AesCbc-PKCS7-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test "PKCS#7" parallel)
  add_test(AesCbc-zero-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test zero sequential)
  add_test(AesCbc-zero-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test zero parallel)
  add_test(AesCbc-ANSIX923-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test ANSIX923 sequential)
  add_test(AesCbc-ANSIX923-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test ANSIX923 parallel)

  add_test(AesPcbc-ISO7816_4 ${EXECUTABLE_OUTPUT_PATH}/anch-aes-pcbc-test ISO7816_4)
  add_test(AesPcbc-PKCS7 ${EXECUTABLE_OUTPUT_PATH}/anch-aes-pcbc-test "PKCS#7")
  add_test(AesPcbc-zero ${EXECUTABLE_OUTPUT_PATH}/anch-aes-pcbc-test zero)
  add_test(AesPcbc-ANSIX923 ${EXECUTABLE_OUTPUT_PATH}/anch-aes-pcbc-test ANSIX923)

  add_test(Aes128Ctr-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ctr-test 128 sequential)
  add_test(Aes128Ctr-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ctr-test 128 parallel)
  add_test(Aes192Ctr-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ctr-test 256 sequential)
  add_test(Aes192Ctr-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ctr-test 192 parallel)
  add_test(Aes256Ctr-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ctr-test 192 sequential)
  add_test(Aes256Ctr-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ctr-test 256 parallel)

  add_test(AesOfb ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ofb-test main)

  add_test(Aes128Cfb-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cfb-test 128 sequential)
  add_test(Aes128Cfb-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cfb-test 128 parallel)
  add_test(Aes192Cfb-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cfb-test 192 sequential)
  add_test(Aes192Cfb-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cfb-test 192 parallel)
  add_test(Aes256Cfb-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cfb-test 256 sequential)
  add_test(Aes256Cfb-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cfb-test 256 parallel)

  find_package(OpenSSL)
  if(OpenSSL_FOUND)
    message(STATUS "OpenSSL include directory: " ${OPENSSL_INCLUDE_DIR})
    message(STATUS "OpenSSL libraries: " ${OPENSSL_LIBRARIES})
    # Build AES benchmark +
    file(
      GLOB_RECURSE
      bench_anch_openssl
      inc/*
      test/benchAnchOpenssl.cpp
    )

    add_executable(
      anch-bench-openssl
      ${bench_anch_openssl}
    )

    target_link_libraries(
      anch-bench-openssl
      anch-crypto
      ${OPENSSL_LIBRARIES}
      ${CMAKE_THREAD_LIBS_INIT}
      )

    add_test(BenchOpenssl ${EXECUTABLE_OUTPUT_PATH}/anch-bench-openssl main)
    # Build AES benchmark -


    # Build AES - CTR unit tests +
    file(
      GLOB_RECURSE
      test_aes_ctr_big_data
      inc/*
      test/testAesCtrBigData.cpp
      )

    add_executable(
      anch-aes-ctr-big-data-test
      ${test_aes_ctr_big_data}
      )

    target_link_libraries(
      anch-aes-ctr-big-data-test
      anch-crypto
      ${OPENSSL_LIBRARIES}
      ${CMAKE_THREAD_LIBS_INIT}
      )
    # Build AES-CTR unit tests -

  endif()

endif(ANCH_CRYPTO_UNITARY_TEST)
