# AnCH CSV: CSV records reading and writing (RFC 4180), objects mapping on their description (anch-utils mapping.hpp)
set(csv_public_deps anch::utils anch::val)
set(csv_public_definitions)
if(ANCH_UUID)
  list(APPEND csv_public_deps anch::uuid)
  list(APPEND csv_public_definitions ANCH_UUID)
endif()
if(ANCH_DATE)
  list(APPEND csv_public_deps anch::date)
  list(APPEND csv_public_definitions ANCH_DATE)
endif()

anch_add_library(anch-csv
  PUBLIC_DEPS ${csv_public_deps}
  PUBLIC_DEFINITIONS ${csv_public_definitions})

if(ANCH_BUILD_TESTS)
  add_subdirectory(test)
endif()
if(ANCH_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
