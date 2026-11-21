# AnCH validation: fields constraints shared by formats (anch-json, ...) and schema generators
set(val_private_deps)
set(val_private_definitions)
# uuid, date and date-time formats
if(ANCH_UUID)
  list(APPEND val_private_deps anch::uuid)
  list(APPEND val_private_definitions ANCH_UUID)
endif()
if(ANCH_DATE)
  list(APPEND val_private_deps anch::date)
  list(APPEND val_private_definitions ANCH_DATE)
endif()

anch_add_library(anch-val
  PRIVATE_DEPS ${val_private_deps}
  PRIVATE_DEFINITIONS ${val_private_definitions})

if(ANCH_BUILD_TESTS)
  add_subdirectory(test)
endif()
if(ANCH_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
