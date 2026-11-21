set_property(DIRECTORY PROPERTY LABELS oas)

# Models and endpoints generated from petstore.json at build time (as in anch-oas documentation) +
set(models_dir ${CMAKE_CURRENT_BINARY_DIR}/models)
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
  COMMENT "Generating petstore models of anch-oas examples")
add_custom_target(anch-oas-examples-models DEPENDS ${models_dir}/petstore.stamp)
# Models and endpoints generated from petstore.json at build time -

anch_add_example(oas-models SOURCES models.cpp ${models_dir}/petstore.cpp LIBS anch::json anch::uuid RUN)
add_dependencies(anch-example-oas-models anch-oas-examples-models)
target_include_directories(anch-example-oas-models PRIVATE ${models_dir})

if(TARGET anch::rest-server AND TARGET anch::rest-client AND ANCH_REST_CURL_ENABLED)
  anch_add_example(oas-endpoints SOURCES endpoints.cpp ${models_dir}/petstore.cpp ${models_dir}/petstore-server.cpp
    ${models_dir}/petstore-client.cpp ${models_dir}/petstore-forms.cpp
    LIBS anch::rest-server anch::rest-client anch::rest-curl anch::uuid RUN)
  add_dependencies(anch-example-oas-endpoints anch-oas-examples-models)
  target_include_directories(anch-example-oas-endpoints PRIVATE ${models_dir})
endif()
