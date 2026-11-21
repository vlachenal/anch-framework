# AnCH CLI: command line arguments parsing and console formatting
anch_add_library(anch-cli
  PRIVATE_DEPS anch::utils) # anch::tryConvert (header only)

if(ANCH_BUILD_TESTS)
  add_subdirectory(test)
endif()
