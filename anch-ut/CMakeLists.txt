# AnCH unit tests framework (provides main())
anch_add_library(anch-ut
  OUTPUT_NAME anch-unit
  PUBLIC_DEPS anch::events anch::network
  PRIVATE_DEPS anch::cli)

if(ANCH_BUILD_TESTS)
  add_subdirectory(test)
endif()
