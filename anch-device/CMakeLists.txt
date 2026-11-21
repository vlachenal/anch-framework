# AnCH device: CPU features, network interfaces and UUID provider
anch_add_library(anch-device
  PUBLIC_DEPS anch::utils
  PRIVATE_DEPS anch::uuid)

if(ANCH_BUILD_TESTS)
  add_subdirectory(test)
endif()
if(ANCH_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
