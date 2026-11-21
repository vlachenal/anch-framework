# AnCH JSON: JSON serialization and deserialization
set(json_public_deps anch::events anch::utils anch::val)
set(json_public_definitions)
if(ANCH_UUID)
  list(APPEND json_public_deps anch::uuid)
  list(APPEND json_public_definitions ANCH_UUID)
endif()
if(ANCH_DATE)
  list(APPEND json_public_deps anch::date)
  list(APPEND json_public_definitions ANCH_DATE)
endif()

anch_add_library(anch-json
  PUBLIC_DEPS ${json_public_deps}
  PUBLIC_DEFINITIONS ${json_public_definitions})

if(ANCH_JSON_UTILS)
  anch_add_executable(anch-json-formatter SOURCES utils/jsonFormat.cpp
    LIBS anch::json anch::cli)
  anch_install_executable(anch-json-formatter)
endif()

if(ANCH_BUILD_TESTS)
  add_subdirectory(test)
endif()
if(ANCH_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
