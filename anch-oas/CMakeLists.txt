# AnCH OAS: OpenAPI 3.0 document parsing and code generation (models for now)
anch_add_library(anch-oas
  PUBLIC_DEPS anch::json)

# Models generator +
anch_add_executable(anch-oas-gen SOURCES utils/oasGen.cpp
  LIBS anch::oas anch::cli)
target_compile_definitions(anch-oas-gen PRIVATE ANCH_OAS_VERSION="${PROJECT_VERSION}")
install(TARGETS anch-oas-gen RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
# Models generator -

if(ANCH_BUILD_TESTS)
  add_subdirectory(test)
endif()
set_property(DIRECTORY PROPERTY LABELS oas)
