# AnCH CLI: command line arguments parsing and console formatting
anch_add_library(anch-cli)

if(ANCH_BUILD_TESTS)
  add_subdirectory(test)
endif()
