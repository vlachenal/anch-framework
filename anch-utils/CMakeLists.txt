# AnCH utils: header-only utilities (thread pool, resource pool, singleton, flux, ...)
anch_add_library(anch-utils HEADER_ONLY
  PUBLIC_DEPS Threads::Threads)

if(ANCH_BUILD_TESTS)
  add_subdirectory(test)
endif()
if(ANCH_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
