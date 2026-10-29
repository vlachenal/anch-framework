# AnCH configuration: configuration loading with profiles, placeholders and resolvers
anch_add_library(anch-conf
  PUBLIC_DEPS anch::ini anch::events anch::utils)

if(ANCH_BUILD_TESTS)
  add_subdirectory(test)
endif()
