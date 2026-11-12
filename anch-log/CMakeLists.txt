# AnCH logger: configurable loggers and writers
# Compile-time level +
string(TOUPPER "${ANCH_LOG_MIN_LEVEL}" log_min_level)
set(log_levels TRACE DEBUG INFO WARN ERROR FATAL)
if(NOT log_min_level IN_LIST log_levels)
  message(FATAL_ERROR "Invalid ANCH_LOG_MIN_LEVEL '${ANCH_LOG_MIN_LEVEL}': expected one of ${log_levels}")
endif()
# TRACE is log/stream.hpp default value: no definition, so that a target can still set its own threshold
set(log_public_definitions)
if(NOT log_min_level STREQUAL "TRACE")
  list(APPEND log_public_definitions ANCH_LOG_MIN_LEVEL=${log_min_level})
endif()
# Compile-time level -

anch_add_library(anch-log
  OUTPUT_NAME anch-logger
  PUBLIC_DEPS anch::conf anch::date anch::utils
  PRIVATE_DEPS anch::cli
  PUBLIC_DEFINITIONS ${log_public_definitions})

if(ANCH_BUILD_TESTS)
  add_subdirectory(test)
endif()
