# AnCH events: header-only observer pattern and event bus
anch_add_library(anch-events HEADER_ONLY
  PUBLIC_DEPS anch::utils)

if(ANCH_BUILD_TESTS)
  add_subdirectory(test)
endif()
