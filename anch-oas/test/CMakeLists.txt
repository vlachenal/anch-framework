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
  COMMAND anch-oas-gen -i ${CMAKE_CURRENT_SOURCE_DIR}/petstore.json -o ${models_dir} -n petstore::model -f petstore
  COMMAND ${CMAKE_COMMAND} -E touch ${models_dir}/petstore.stamp
  DEPENDS anch-oas-gen ${CMAKE_CURRENT_SOURCE_DIR}/petstore.json
  COMMENT "Generating petstore models")
add_custom_target(anch-oas-petstore-models DEPENDS ${models_dir}/petstore.stamp)
anch_add_executable(anch-oas-models-test SOURCES testModels.cpp ${models_dir}/petstore.cpp
  LIBS anch::json anch::uuid anch::ut)
add_dependencies(anch-oas-models-test anch-oas-petstore-models)
target_include_directories(anch-oas-models-test PRIVATE ${models_dir})
# Models generated from petstore.json at build time -

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
add_test(NAME oas-models-date-invalid COMMAND anch-oas-models-test -t models-date-invalid)
