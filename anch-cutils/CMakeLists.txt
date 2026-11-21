# AnCH C utils: C++ streams on top of C read/write functions
anch_add_library(anch-cutils)

if(ANCH_BUILD_TESTS)
  add_subdirectory(test)
endif()
if(ANCH_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
