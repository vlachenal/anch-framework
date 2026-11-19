set_property(DIRECTORY PROPERTY LABELS oas)

# Generator tests +
anch_add_executable(anch-oas-generator-test SOURCES testGenerator.cpp LIBS anch::oas anch::ut)
# Generator tests -

# Models generated from petstore.json at build time +
# anch-oas-gen does not rewrite unchanged files: a stamp file tracks the generation
set(models_dir ${CMAKE_CURRENT_BINARY_DIR}/gen)
add_custom_command(
  OUTPUT ${models_dir}/petstore.stamp
  BYPRODUCTS ${models_dir}/petstore.hpp ${models_dir}/petstore.cpp
    ${models_dir}/petstore-server.hpp ${models_dir}/petstore-server.cpp
    ${models_dir}/petstore-client.hpp ${models_dir}/petstore-client.cpp
    ${models_dir}/petstore-forms.hpp ${models_dir}/petstore-forms.cpp
  COMMAND anch-oas-gen -i ${CMAKE_CURRENT_SOURCE_DIR}/petstore.json -o ${models_dir} -n petstore::model -f petstore
    --server --client
  COMMAND ${CMAKE_COMMAND} -E touch ${models_dir}/petstore.stamp
  DEPENDS anch-oas-gen ${CMAKE_CURRENT_SOURCE_DIR}/petstore.json
  COMMENT "Generating petstore models")
add_custom_target(anch-oas-petstore-models DEPENDS ${models_dir}/petstore.stamp)
anch_add_executable(anch-oas-models-test SOURCES testModels.cpp ${models_dir}/petstore.cpp
  LIBS anch::json anch::uuid anch::ut)
add_dependencies(anch-oas-models-test anch-oas-petstore-models)
target_include_directories(anch-oas-models-test PRIVATE ${models_dir})
# Models generated from petstore.json at build time -

# Endpoints generated from petstore.json (anch-rest server and client) +
if(TARGET anch::rest-server AND TARGET anch::rest-client)
  anch_add_executable(anch-oas-endpoints-test SOURCES testEndpoints.cpp ${models_dir}/petstore.cpp
    ${models_dir}/petstore-server.cpp ${models_dir}/petstore-client.cpp ${models_dir}/petstore-forms.cpp
    LIBS anch::rest-server anch::rest-client anch::uuid anch::ut)
  add_dependencies(anch-oas-endpoints-test anch-oas-petstore-models)
  target_include_directories(anch-oas-endpoints-test PRIVATE ${models_dir})
  add_test(NAME oas-endpoints-server COMMAND anch-oas-endpoints-test -t endpoints-server)
  add_test(NAME oas-endpoints-client COMMAND anch-oas-endpoints-test -t endpoints-client)
endif()
# Endpoints generated from petstore.json (anch-rest server and client) -

# Form bodies generated from forms.json (anch-rest server and client) +
if(TARGET anch::rest-server AND TARGET anch::rest-client)
  add_custom_command(
    OUTPUT ${models_dir}/photos.stamp
    BYPRODUCTS ${models_dir}/photos.hpp ${models_dir}/photos.cpp
      ${models_dir}/photos-server.hpp ${models_dir}/photos-server.cpp
      ${models_dir}/photos-client.hpp ${models_dir}/photos-client.cpp
      ${models_dir}/photos-forms.hpp ${models_dir}/photos-forms.cpp
    COMMAND anch-oas-gen -i ${CMAKE_CURRENT_SOURCE_DIR}/forms.json -o ${models_dir} -n photos::model -f photos --server --client
    COMMAND ${CMAKE_COMMAND} -E touch ${models_dir}/photos.stamp
    DEPENDS anch-oas-gen ${CMAKE_CURRENT_SOURCE_DIR}/forms.json
    COMMENT "Generating pet photos models")
  add_custom_target(anch-oas-photos-models DEPENDS ${models_dir}/photos.stamp)
  anch_add_executable(anch-oas-forms-test SOURCES testForms.cpp ${models_dir}/photos.cpp
    ${models_dir}/photos-server.cpp ${models_dir}/photos-client.cpp ${models_dir}/photos-forms.cpp
    LIBS anch::rest-server anch::rest-client anch::ut)
  add_dependencies(anch-oas-forms-test anch-oas-photos-models)
  target_include_directories(anch-oas-forms-test PRIVATE ${models_dir})
  add_test(NAME oas-forms-server COMMAND anch-oas-forms-test -t forms-server)
  add_test(NAME oas-forms-client COMMAND anch-oas-forms-test -t forms-client)
endif()
# Form bodies generated from forms.json (anch-rest server and client) -

add_test(NAME oas-spec-version COMMAND anch-oas-generator-test -t spec-version)
add_test(NAME oas-spec-invalid-json COMMAND anch-oas-generator-test -t spec-invalid-json)
add_test(NAME oas-gen-unknown-reference COMMAND anch-oas-generator-test -t gen-unknown-reference)
add_test(NAME oas-gen-external-reference COMMAND anch-oas-generator-test -t gen-external-reference)
add_test(NAME oas-gen-strict COMMAND anch-oas-generator-test -t gen-strict)
add_test(NAME oas-gen-options COMMAND anch-oas-generator-test -t gen-options)
add_test(NAME oas-gen-member-names COMMAND anch-oas-generator-test -t gen-member-names)
add_test(NAME oas-gen-type-names COMMAND anch-oas-generator-test -t gen-type-names)
add_test(NAME oas-gen-enum-values COMMAND anch-oas-generator-test -t gen-enum-values)
add_test(NAME oas-gen-recursive-alias COMMAND anch-oas-generator-test -t gen-recursive-alias)
add_test(NAME oas-gen-inherited-warnings COMMAND anch-oas-generator-test -t gen-inherited-warnings)
add_test(NAME oas-gen-recursive-allof COMMAND anch-oas-generator-test -t gen-recursive-allof)
add_test(NAME oas-gen-no-uuid COMMAND anch-oas-generator-test -t gen-no-uuid)
add_test(NAME oas-gen-no-date COMMAND anch-oas-generator-test -t gen-no-date)
add_test(NAME oas-gen-pointers COMMAND anch-oas-generator-test -t gen-pointers)
add_test(NAME oas-gen-required COMMAND anch-oas-generator-test -t gen-required)
add_test(NAME oas-gen-constraints COMMAND anch-oas-generator-test -t gen-constraints)
add_test(NAME oas-gen-constraints-warnings COMMAND anch-oas-generator-test -t gen-constraints-warnings)
add_test(NAME oas-gen-constraints-inconsistent COMMAND anch-oas-generator-test -t gen-constraints-inconsistent)
add_test(NAME oas-gen-trigraphs COMMAND anch-oas-generator-test -t gen-trigraphs)
add_test(NAME oas-gen-endpoints COMMAND anch-oas-generator-test -t gen-endpoints)
add_test(NAME oas-gen-endpoints-warnings COMMAND anch-oas-generator-test -t gen-endpoints-warnings)
add_test(NAME oas-gen-forms COMMAND anch-oas-generator-test -t gen-forms)
add_test(NAME oas-gen-raw-body COMMAND anch-oas-generator-test -t gen-raw-body)
add_test(NAME oas-models-pet COMMAND anch-oas-models-test -t models-pet)
add_test(NAME oas-models-optional COMMAND anch-oas-models-test -t models-optional)
add_test(NAME oas-models-nullable-enum COMMAND anch-oas-models-test -t models-nullable-enum)
add_test(NAME oas-models-enum-values COMMAND anch-oas-models-test -t models-enum-values)
add_test(NAME oas-models-enum-invalid COMMAND anch-oas-models-test -t models-enum-invalid)
add_test(NAME oas-models-unknown-field COMMAND anch-oas-models-test -t models-unknown-field)
add_test(NAME oas-models-allof COMMAND anch-oas-models-test -t models-allof)
add_test(NAME oas-models-mutual-recursion COMMAND anch-oas-models-test -t models-mutual-recursion)
add_test(NAME oas-models-aliases COMMAND anch-oas-models-test -t models-aliases)
add_test(NAME oas-models-empty COMMAND anch-oas-models-test -t models-empty)
add_test(NAME oas-models-round-trip COMMAND anch-oas-models-test -t models-round-trip)
add_test(NAME oas-models-required-missing COMMAND anch-oas-models-test -t models-required-missing)
add_test(NAME oas-models-required-null COMMAND anch-oas-models-test -t models-required-null)
add_test(NAME oas-models-required-nullable COMMAND anch-oas-models-test -t models-required-nullable)
add_test(NAME oas-models-constraints COMMAND anch-oas-models-test -t models-constraints)
add_test(NAME oas-models-constraints-invalid COMMAND anch-oas-models-test -t models-constraints-invalid)
add_test(NAME oas-models-required-serialize COMMAND anch-oas-models-test -t models-required-serialize)
add_test(NAME oas-models-constraints-serialize COMMAND anch-oas-models-test -t models-constraints-serialize)
add_test(NAME oas-models-validate COMMAND anch-oas-models-test -t models-validate)
add_test(NAME oas-models-date-invalid COMMAND anch-oas-models-test -t models-date-invalid)
