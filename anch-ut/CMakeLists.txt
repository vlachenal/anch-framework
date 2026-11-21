# AnCH unit tests framework (provides main())
anch_add_library(anch-ut
  OUTPUT_NAME anch-unit
  PRIVATE_DEPS anch::cli)

if(ANCH_BUILD_TESTS)
  add_subdirectory(test)
endif()
if(ANCH_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
