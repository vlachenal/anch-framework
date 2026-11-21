# AnCH CSV: CSV records reading and writing (RFC 4180)
anch_add_library(anch-csv
  PUBLIC_DEPS anch::utils)

if(ANCH_BUILD_TESTS)
  add_subdirectory(test)
endif()
if(ANCH_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
