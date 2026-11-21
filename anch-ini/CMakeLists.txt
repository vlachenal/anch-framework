# AnCH INI: INI files parser
anch_add_library(anch-ini
  PRIVATE_DEPS anch::utils) # anch::tryConvert (header only)

if(ANCH_BUILD_TESTS)
  add_subdirectory(test)
endif()
if(ANCH_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
