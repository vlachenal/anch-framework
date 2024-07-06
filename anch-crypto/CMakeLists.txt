cmake_minimum_required(VERSION 3.10)

# Project anch crypto
# ANother C++ Harsh/Huge/Hack
project(anch-crypto)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include(AnchCrypto.cmake.in)

# Check GCC version +
execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                OUTPUT_VARIABLE GCC_VERSION)
if (GCC_VERSION VERSION_LESS 10.0)
  message(FATAL_ERROR "Version < 10.0. This library is based on C++20 standard. Exit.")
elseif(GCC_VERSION VERSION_GREATER 10.0 OR GCC_VERSION VERSION_EQUAL 10.0)
  message(STATUS "Version >= 10.0 use -std=c++20")
  add_definitions("-std=c++20")
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
add_definitions(${ANCH_CRYPTO_COMPILE_FLAGS})

if(ANCH_CPU_AES)
  add_definitions("-msse4 -maes -D ANCH_CPU_AES")
elseif(ANCH_CPU_DETECTION)
  add_definitions("-msse4 -maes -D ANCH_CPU_AES -D ANCH_CPU_DETECTION")
endif()

# Includes location
if(ANCH_CPU_DETECTION)
  include_directories(
    inc
    ${ANCH_UUID_INCLUDES_DIR}
    ${ANCH_UTILS_INCLUDES_DIR}
    ${ANCH_DEVICE_INCLUDES_DIR}
    )
else()
  include_directories(
    inc
    ${ANCH_UUID_INCLUDES_DIR}
    ${ANCH_UTILS_INCLUDES_DIR}
    )
endif()

# Retrieve every files in project
file(
  GLOB_RECURSE
  source_files
  src/*
  )

# Build objects
add_library(
  anch-crypto-obj
  OBJECT
  ${source_files}
  )

## Dynamic library +
add_library(
  anch-crypto
  SHARED
  $<TARGET_OBJECTS:anch-crypto-obj>
)

install(
  DIRECTORY inc/ DESTINATION ${ANCH_CRYPTO_INSTALL_INC_DIR}
  FILES_MATCHING PATTERN "*.hpp"
  )

install(
  TARGETS anch-crypto
  LIBRARY DESTINATION ${ANCH_CRYPTO_INSTALL_LIB_DIR}
  )
## Dynamic library -

## Static library +
if(ANCH_CRYPTO_STATIC_LIB)
  add_library(
    anch-crypto-static
    STATIC
    $<TARGET_OBJECTS:anch-crypto-obj>
    )
  set_target_properties(anch-crypto-static PROPERTIES OUTPUT_NAME anch-crypto)

  install(
    TARGETS anch-crypto-static
    ARCHIVE DESTINATION ${ANCH_CRYPTO_INSTALL_LIB_DIR}
    )
endif(ANCH_CRYPTO_STATIC_LIB)
## Static library -


# Unitary tests
if(ANCH_CRYPTO_UNITARY_TEST)

  include_directories(
    inc
    ${ANCH_UUID_INCLUDES_DIR}
    ${ANCH_UTILS_INCLUDES_DIR}
    ${ANCH_UT_INCLUDES_DIR}
    ${ANCH_CLI_INCLUDES_DIR}
    )

  link_directories(
    ${ANCH_UUID_LIBRARY_DIR}
    ${ANCH_UT_LIBRARY_DIR}
    ${ANCH_CLI_LIBRARY_DIR}
    )

  # Build MD5 unitary test +
  file(
    GLOB_RECURSE
    test_md5
    test/testMD5.cpp
  )

  add_executable(
    anch-md5-test
    ${test_md5}
  )

  target_link_libraries(
    anch-md5-test
    anch-crypto
    anch-cli
    anch-unit
    anch-uuid
  )
  # Build MD5 unitary test -

  # Build SHA1 unitary test +
  file(
    GLOB_RECURSE
    test_sha1
    test/testSHA1.cpp
  )

  add_executable(
    anch-sha1-test
    ${test_sha1}
  )

  target_link_libraries(
    anch-sha1-test
    anch-crypto
    anch-uuid
    anch-cli
    anch-unit
  )
  # Build SHA1 unitary test -

  # Build SHA2 unitary test +
  file(
    GLOB_RECURSE
    test_sha2
    test/testSHA2.cpp
  )

  add_executable(
    anch-sha2-test
    ${test_sha2}
  )

  target_link_libraries(
    anch-sha2-test
    anch-crypto
    anch-cli
    anch-unit
    anch-uuid
  )
  # Build SHA2 unitary test -

  # Build HMAC unitary test +
  file(
    GLOB_RECURSE
    test_hmac
    test/testHmac.cpp
  )

  add_executable(
    anch-hmac-test
    ${test_hmac}
  )

  target_link_libraries(
    anch-hmac-test
    anch-crypto
    anch-cli
    anch-unit
    anch-uuid
  )
  # Build HMAC unitary test -

  # Build Base64 unitary test +
  file(
    GLOB_RECURSE
    test_base64
    test/testBase64.cpp
  )

  add_executable(
    anch-base64-test
    ${test_base64}
  )

  target_link_libraries(
    anch-base64-test
    anch-crypto
    anch-cli
    anch-unit
    anch-uuid
  )
  # Build Base64 unitary test -

  # Build AES unitary test +
  file(
    GLOB_RECURSE
    test_aes
    test/testAES.cpp
  )

  add_executable(
    anch-aes-test
    ${test_aes}
  )

  target_link_libraries(
    anch-aes-test
    anch-crypto
    anch-cli
    anch-unit
    anch-uuid
  )
  # Build AES unitary test -

  # Build padding unitary test +
  file(
    GLOB_RECURSE
    test_padding
    test/testPadding.cpp
  )

  add_executable(
    anch-padding-test
    ${test_padding}
  )

  target_link_libraries(
    anch-padding-test
    anch-crypto
    anch-cli
    anch-unit
    anch-uuid
  )
  # Build padding unitary test -

  # Build AES - ECB unit tests +
  file(
    GLOB_RECURSE
    test_aes_ecb
    test/testAesEcb.cpp
  )

  add_executable(
    anch-aes-ecb-test
    ${test_aes_ecb}
  )

  target_link_libraries(
    anch-aes-ecb-test
    anch-crypto
    anch-cli
    anch-unit
    anch-uuid
  )
  # Build AES-ECB unit tests -

  # Build AES - CBC unit tests +
  file(
    GLOB_RECURSE
    test_aes_cbc
    test/testAesCbc.cpp
  )

  add_executable(
    anch-aes-cbc-test
    ${test_aes_cbc}
  )

  target_link_libraries(
    anch-aes-cbc-test
    anch-crypto
    anch-cli
    anch-unit
    anch-uuid
  )
  # Build AES-CBC unit tests -

  # Build AES - PCBC unit tests +
  file(
    GLOB_RECURSE
    test_aes_pcbc
    test/testAesPcbc.cpp
  )

  add_executable(
    anch-aes-pcbc-test
    ${test_aes_pcbc}
  )

  target_link_libraries(
    anch-aes-pcbc-test
    anch-crypto
    anch-cli
    anch-unit
    anch-uuid
  )
  # Build AES-PCBC unit tests -

  # Build AES - CTR unit tests +
  file(
    GLOB_RECURSE
    test_aes_ctr
    test/testAesCtr.cpp
  )

  add_executable(
    anch-aes-ctr-test
    ${test_aes_ctr}
  )

  target_link_libraries(
    anch-aes-ctr-test
    anch-crypto
    anch-cli
    anch-unit
    anch-uuid
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
    anch-cli
    anch-unit
    anch-uuid
  )
  # Build AES-OFB unit tests -

  # Build AES - CFB unit tests +
  file(
    GLOB_RECURSE
    test_aes_cfb
    test/testAesCfb.cpp
  )

  add_executable(
    anch-aes-cfb-test
    ${test_aes_cfb}
  )

  target_link_libraries(
    anch-aes-cfb-test
    anch-crypto
    anch-cli
    anch-unit
    anch-uuid
  )
  # Build AES-CFB unit tests -

  # Build UUID unit tests +
  file(
    GLOB_RECURSE
    test_uuid
    test/testUuid.cpp
  )

  add_executable(
    anch-crypto-uuid-test
    ${test_uuid}
  )

  target_link_libraries(
    anch-crypto-uuid-test
    anch-crypto
    anch-cli
    anch-unit
    anch-uuid
  )
  # Build UUID unit tests -

  enable_testing()
  # MD5 tests +
  add_test(md5-exorciste ${EXECUTABLE_OUTPUT_PATH}/anch-md5-test --test md5-str)
  add_test(md5-1Ma ${EXECUTABLE_OUTPUT_PATH}/anch-md5-test --test md5-1Ma)
  add_test(md5-makefile ${EXECUTABLE_OUTPUT_PATH}/anch-md5-test --test md5-makefile)
  # MD5 tests -

  # SHA1 tests +
  add_test(sha1-abc ${EXECUTABLE_OUTPUT_PATH}/anch-sha1-test --test sha1-abc)
  add_test(sha1-plop ${EXECUTABLE_OUTPUT_PATH}/anch-sha1-test --test sha1-plop)
  add_test(sha1-1Ma ${EXECUTABLE_OUTPUT_PATH}/anch-sha1-test --test sha1-1Ma)
  add_test(sha1-sec-password ${EXECUTABLE_OUTPUT_PATH}/anch-sha1-test --test sha1-sec-password)
  # SHA1 tests -

  # SHA2 tests +
  add_test(sha2-224 ${EXECUTABLE_OUTPUT_PATH}/anch-sha2-test -t sha2-224)
  add_test(sha2-256 ${EXECUTABLE_OUTPUT_PATH}/anch-sha2-test -t sha2-256)
  add_test(sha2-384 ${EXECUTABLE_OUTPUT_PATH}/anch-sha2-test -t sha2-384)
  add_test(sha2-512 ${EXECUTABLE_OUTPUT_PATH}/anch-sha2-test -t sha2-512)
  # SHA2 tests -

  # HMAC tests +
  add_test(hmac-md5 ${EXECUTABLE_OUTPUT_PATH}/anch-hmac-test -t hmac-md5)
  add_test(hmac-sha1 ${EXECUTABLE_OUTPUT_PATH}/anch-hmac-test -t hmac-sha1)
  add_test(hmac-sha224 ${EXECUTABLE_OUTPUT_PATH}/anch-hmac-test -t hmac-sha224)
  add_test(hmac-sha256 ${EXECUTABLE_OUTPUT_PATH}/anch-hmac-test -t hmac-sha256)
  #add_test(hmac-sha384 ${EXECUTABLE_OUTPUT_PATH}/anch-hmac-test -t hmac-sha384)
  add_test(hmac-sha512 ${EXECUTABLE_OUTPUT_PATH}/anch-hmac-test -t hmac-sha512)
  # HMAC tests -

  # Base64 tests +
  add_test(base64-enc-empty ${EXECUTABLE_OUTPUT_PATH}/anch-base64-test -t base64-enc-empty)
  add_test(base64-enc-f ${EXECUTABLE_OUTPUT_PATH}/anch-base64-test -t base64-enc-f)
  add_test(base64-enc-fo ${EXECUTABLE_OUTPUT_PATH}/anch-base64-test -t base64-enc-fo)
  add_test(base64-enc-foo ${EXECUTABLE_OUTPUT_PATH}/anch-base64-test -t base64-enc-foo)
  add_test(base64-enc-foob ${EXECUTABLE_OUTPUT_PATH}/anch-base64-test -t base64-enc-foob)
  add_test(base64-enc-fooba ${EXECUTABLE_OUTPUT_PATH}/anch-base64-test -t base64-enc-fooba)
  add_test(base64-enc-foobar ${EXECUTABLE_OUTPUT_PATH}/anch-base64-test -t base64-enc-foobar)
  add_test(base64-dec-empty ${EXECUTABLE_OUTPUT_PATH}/anch-base64-test -t base64-dec-empty)
  add_test(base64-dec-f ${EXECUTABLE_OUTPUT_PATH}/anch-base64-test -t base64-dec-f)
  add_test(base64-dec-fo ${EXECUTABLE_OUTPUT_PATH}/anch-base64-test -t base64-dec-fo)
  add_test(base64-dec-foo ${EXECUTABLE_OUTPUT_PATH}/anch-base64-test -t base64-dec-foo)
  add_test(base64-dec-foob ${EXECUTABLE_OUTPUT_PATH}/anch-base64-test -t base64-dec-foob)
  add_test(base64-dec-fooba ${EXECUTABLE_OUTPUT_PATH}/anch-base64-test -t base64-dec-fooba)
  add_test(base64-dec-foobar ${EXECUTABLE_OUTPUT_PATH}/anch-base64-test -t base64-dec-foobar)
  add_test(base64-enc-makefile ${EXECUTABLE_OUTPUT_PATH}/anch-base64-test -t base64-enc-makefile)
  # Base64 tests -

  # AES single block tests +
  add_test(AES128 ${EXECUTABLE_OUTPUT_PATH}/anch-aes-test -t 128)
  add_test(AES192 ${EXECUTABLE_OUTPUT_PATH}/anch-aes-test -t 192)
  add_test(AES256 ${EXECUTABLE_OUTPUT_PATH}/anch-aes-test -t 256)
  # AES single block tests -

  # Paddings tests +
  add_test(Padding-ANSIX923 ${EXECUTABLE_OUTPUT_PATH}/anch-padding-test -t ANSIX923)
  add_test(Padding-PKCS7 ${EXECUTABLE_OUTPUT_PATH}/anch-padding-test -t PKCS7)
  add_test(Padding-ISO7816_4 ${EXECUTABLE_OUTPUT_PATH}/anch-padding-test -t ISO7816_4)
  add_test(Padding-zero ${EXECUTABLE_OUTPUT_PATH}/anch-padding-test -t zero)
  # Paddings tests -

  # AES ECB tests +
  add_test(Aes128Ecb-ISO7816_4-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test -t ISO7816_4-sequential-128)
  add_test(Aes128Ecb-ISO7816_4-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test -t ISO7816_4-parallel-128)
  add_test(Aes128Ecb-PKCS7-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test -t PKCS7-sequential-128)
  add_test(Aes128Ecb-PKCS7-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test -t PKCS7-parallel-128)
  add_test(Aes128Ecb-zero-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test -t 0-sequential-128)
  add_test(Aes128Ecb-zero-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test -t 0-parallel-128)
  add_test(Aes128Ecb-ANSIX923-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test -t ANSIX923-sequential-128)
  add_test(Aes128Ecb-ANSIX923-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test -t ANSIX923-parallel-128)

  add_test(Aes192Ecb-ISO7816_4-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test -t ISO7816_4-sequential-192)
  add_test(Aes192Ecb-ISO7816_4-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test -t ISO7816_4-parallel-192)
  add_test(Aes192Ecb-PKCS7-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test -t PKCS7-sequential-192)
  add_test(Aes192Ecb-PKCS7-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test -t PKCS7-parallel-192)
  add_test(Aes192Ecb-zero-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test -t 0-sequential-192)
  add_test(Aes192Ecb-zero-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test -t 0-parallel-192)
  add_test(Aes192Ecb-ANSIX923-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test -t ANSIX923-sequential-192)
  add_test(Aes192Ecb-ANSIX923-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test -t ANSIX923-parallel-192)

  add_test(Aes256Ecb-ISO7816_4-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test -t ISO7816_4-sequential-256)
  add_test(Aes256Ecb-ISO7816_4-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test -t ISO7816_4-parallel-256)
  add_test(Aes256Ecb-PKCS7-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test -t PKCS7-sequential-256)
  add_test(Aes256Ecb-PKCS7-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test -t PKCS7-parallel-256)
  add_test(Aes256Ecb-zero-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test -t 0-sequential-256)
  add_test(Aes256Ecb-zero-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test -t 0-parallel-256)
  add_test(Aes256Ecb-ANSIX923-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test -t ANSIX923-sequential-256)
  add_test(Aes256Ecb-ANSIX923-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ecb-test -t ANSIX923-parallel-256)
  # AES ECB tests -

  # AES CBC tests +
  add_test(Aes128Cbc-ISO7816_4-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test -t ISO7816_4-sequential-128)
  add_test(Aes128Cbc-ISO7816_4-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test -t ISO7816_4-parallel-128)
  add_test(Aes128Cbc-PKCS7-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test -t PKCS7-sequential-128)
  add_test(Aes128Cbc-PKCS7-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test -t PKCS7-parallel-128)
  add_test(Aes128Cbc-zero-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test -t 0-sequential-128)
  add_test(Aes128Cbc-zero-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test -t 0-parallel-128)
  add_test(Aes128Cbc-ANSIX923-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test -t ANSIX923-sequential-128)
  add_test(Aes128Cbc-ANSIX923-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test -t ANSIX923-parallel-128)

  add_test(Aes192Cbc-ISO7816_4-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test -t ISO7816_4-sequential-192)
  add_test(Aes192Cbc-ISO7816_4-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test -t ISO7816_4-parallel-192)
  add_test(Aes192Cbc-PKCS7-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test -t PKCS7-sequential-192)
  add_test(Aes192Cbc-PKCS7-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test -t PKCS7-parallel-192)
  add_test(Aes192Cbc-zero-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test -t 0-sequential-192)
  add_test(Aes192Cbc-zero-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test -t 0-parallel-192)
  add_test(Aes192Cbc-ANSIX923-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test -t ANSIX923-sequential-192)
  add_test(Aes192Cbc-ANSIX923-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test -t ANSIX923-parallel-192)

  add_test(Aes256Cbc-ISO7816_4-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test -t ISO7816_4-sequential-256)
  add_test(Aes256Cbc-ISO7816_4-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test -t ISO7816_4-parallel-256)
  add_test(Aes256Cbc-PKCS7-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test -t PKCS7-sequential-256)
  add_test(Aes256Cbc-PKCS7-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test -t PKCS7-parallel-256)
  add_test(Aes256Cbc-zero-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test -t 0-sequential-256)
  add_test(Aes256Cbc-zero-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test -t 0-parallel-256)
  add_test(Aes256Cbc-ANSIX923-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test -t ANSIX923-sequential-256)
  add_test(Aes256Cbc-ANSIX923-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cbc-test -t ANSIX923-parallel-256)
  # AES CBC tests -

  # AES PCBC tests +
  add_test(Aes128Pcbc-ISO7816_4 ${EXECUTABLE_OUTPUT_PATH}/anch-aes-pcbc-test -t ISO7816_4-128)
  add_test(Aes128Pcbc-PKCS7 ${EXECUTABLE_OUTPUT_PATH}/anch-aes-pcbc-test -t PKCS7-128)
  add_test(Aes128Pcbc-zero ${EXECUTABLE_OUTPUT_PATH}/anch-aes-pcbc-test -t 0-128)
  add_test(Aes128Pcbc-ANSIX923 ${EXECUTABLE_OUTPUT_PATH}/anch-aes-pcbc-test -t ANSIX923-128)

  add_test(Aes192Pcbc-ISO7816_4 ${EXECUTABLE_OUTPUT_PATH}/anch-aes-pcbc-test -t ISO7816_4-192)
  add_test(Aes192Pcbc-PKCS7 ${EXECUTABLE_OUTPUT_PATH}/anch-aes-pcbc-test -t PKCS7-192)
  add_test(Aes192Pcbc-zero ${EXECUTABLE_OUTPUT_PATH}/anch-aes-pcbc-test -t 0-192)
  add_test(Aes192Pcbc-ANSIX923 ${EXECUTABLE_OUTPUT_PATH}/anch-aes-pcbc-test -t ANSIX923-192)

  add_test(Aes256Pcbc-ISO7816_4 ${EXECUTABLE_OUTPUT_PATH}/anch-aes-pcbc-test -t ISO7816_4-256)
  add_test(Aes256Pcbc-PKCS7 ${EXECUTABLE_OUTPUT_PATH}/anch-aes-pcbc-test -t PKCS7-256)
  add_test(Aes256Pcbc-zero ${EXECUTABLE_OUTPUT_PATH}/anch-aes-pcbc-test -t 0-256)
  add_test(Aes256Pcbc-ANSIX923 ${EXECUTABLE_OUTPUT_PATH}/anch-aes-pcbc-test -t ANSIX923-256)
  # AES PCBC tests -

  # AES CTR tests +
  add_test(Aes128Ctr-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ctr-test -t sequential-128)
  add_test(Aes128Ctr-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ctr-test -t parallel-128)

  add_test(Aes192Ctr-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ctr-test -t sequential-192)
  add_test(Aes192Ctr-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ctr-test -t parallel-192)

  add_test(Aes256Ctr-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ctr-test -t sequential-256)
  add_test(Aes256Ctr-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ctr-test -t parallel-256)
  # AES CTR tests -

  # AES OFB tests +
  add_test(Aes128Ofb ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ofb-test -t 128)
  add_test(Aes192Ofb ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ofb-test -t 192)
  add_test(Aes256Ofb ${EXECUTABLE_OUTPUT_PATH}/anch-aes-ofb-test -t 256)
  # AES OFB tests -

  # AES OFB tests +
  add_test(Aes128Cfb-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cfb-test -t 128-sequential)
  add_test(Aes128Cfb-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cfb-test -t 128-parallel)

  add_test(Aes192Cfb-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cfb-test -t 192-sequential)
  add_test(Aes192Cfb-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cfb-test -t 192-parallel)

  add_test(Aes256Cfb-seq ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cfb-test -t 256-sequential)
  add_test(Aes256Cfb-para ${EXECUTABLE_OUTPUT_PATH}/anch-aes-cfb-test -t 256-parallel)
  # AES OFB tests -

  # UUID tests +
  add_test(UUID-MD5 ${EXECUTABLE_OUTPUT_PATH}/anch-crypto-uuid-test -t MD5)
  add_test(UUID-SHA1 ${EXECUTABLE_OUTPUT_PATH}/anch-crypto-uuid-test -t SHA1)
  # UUID tests -

  find_package(OpenSSL)
  if(OpenSSL_FOUND)
    message(STATUS "OpenSSL include directory: " ${OPENSSL_INCLUDE_DIR})
    message(STATUS "OpenSSL libraries: " ${OPENSSL_LIBRARIES})
    # Build AES benchmark +
    file(
      GLOB_RECURSE
      bench_anch_openssl
      test/benchAnchOpenssl.cpp
    )

    add_executable(
      anch-bench-openssl
      ${bench_anch_openssl}
    )

    target_link_libraries(
      anch-bench-openssl
      anch-crypto
      anch-uuid
      ${OPENSSL_LIBRARIES}
      )

    add_test(BenchOpenssl ${EXECUTABLE_OUTPUT_PATH}/anch-bench-openssl main)
    # Build AES benchmark -


    # Build AES - CTR unit tests +
    file(
      GLOB_RECURSE
      test_aes_ctr_big_data
      test/testAesCtrBigData.cpp
      )

    add_executable(
      anch-aes-ctr-big-data-test
      ${test_aes_ctr_big_data}
      )

    target_link_libraries(
      anch-aes-ctr-big-data-test
      anch-crypto
      anch-uuid
      ${OPENSSL_LIBRARIES}
      )
    # Build AES-CTR unit tests -

  endif()

endif(ANCH_CRYPTO_UNITARY_TEST)
