# AnCH crypto: hash, HMAC, base64, AES and block cipher modes
set(crypto_public_deps anch::utils)
set(crypto_public_definitions)
set(crypto_public_options)
if(ANCH_CPU_AES)
  list(APPEND crypto_public_definitions ANCH_CPU_AES)
  list(APPEND crypto_public_options -msse4 -maes)
elseif(ANCH_CPU_DETECTION)
  list(APPEND crypto_public_deps anch::device)
  list(APPEND crypto_public_definitions ANCH_CPU_AES ANCH_CPU_DETECTION)
  list(APPEND crypto_public_options -msse4 -maes)
endif()

anch_add_library(anch-crypto
  PUBLIC_DEPS ${crypto_public_deps}
  PRIVATE_DEPS anch::uuid
  PUBLIC_DEFINITIONS ${crypto_public_definitions})
# AES intrinsics are used in public headers
target_compile_options(anch-crypto PUBLIC ${crypto_public_options})

if(ANCH_BUILD_TESTS)
  add_subdirectory(test)
endif()
if(ANCH_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
