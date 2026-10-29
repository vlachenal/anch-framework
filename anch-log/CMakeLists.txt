# AnCH logger: configurable loggers and writers
anch_add_library(anch-log
  OUTPUT_NAME anch-logger
  PUBLIC_DEPS anch::conf anch::date anch::utils
  PRIVATE_DEPS anch::cli)

if(ANCH_BUILD_TESTS)
  add_subdirectory(test)
endif()
