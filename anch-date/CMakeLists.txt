# AnCH date: date representation and formatting
anch_add_library(anch-date)

if(ANCH_BUILD_TESTS)
  add_subdirectory(test)
endif()
if(ANCH_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
