set_property(DIRECTORY PROPERTY LABELS codec)

anch_add_executable(anch-codec-test SOURCES testCodec.cpp LIBS anch::codec anch::ut)

add_test(NAME anch-codec-media-type COMMAND anch-codec-test -t media-type)
add_test(NAME anch-codec-accept COMMAND anch-codec-test -t accept)
add_test(NAME anch-codec-dispatch COMMAND anch-codec-test -t dispatch)
add_test(NAME anch-codec-limits COMMAND anch-codec-test -t limits)
add_test(NAME anch-codec-text COMMAND anch-codec-test -t text)
add_test(NAME anch-codec-codecs COMMAND anch-codec-test -t codecs)
add_test(NAME anch-codec-formats COMMAND anch-codec-test -t formats)

if(ANCH_CODEC_JSON_ENABLED)
  anch_add_executable(anch-codec-json-test SOURCES testJson.cpp LIBS anch::codec-all anch::ut)

  add_test(NAME anch-codec-json-serialize COMMAND anch-codec-json-test -t serialize)
  add_test(NAME anch-codec-json-constraints COMMAND anch-codec-json-test -t constraints)
  add_test(NAME anch-codec-json-too-large COMMAND anch-codec-json-test -t too-large)
  add_test(NAME anch-codec-json-available COMMAND anch-codec-json-test -t available)
  add_test(NAME anch-codec-json-formats COMMAND anch-codec-json-test -t formats)
endif()

# Plugins with an external schema: Protobuf messages and Thrift structures generated for the tests +
set(schema_libs)
if(ANCH_CODEC_PROTOBUF_ENABLED)
  set(proto_gen ${CMAKE_CURRENT_BINARY_DIR}/proto-gen)
  file(MAKE_DIRECTORY ${proto_gen})
  add_library(anch-codec-test-pet-proto STATIC schema/pet.proto)
  anch_target_build_options(anch-codec-test-pet-proto)
  target_link_libraries(anch-codec-test-pet-proto PUBLIC anch::codec-protobuf)
  target_include_directories(anch-codec-test-pet-proto SYSTEM PUBLIC ${proto_gen})
  protobuf_generate(TARGET anch-codec-test-pet-proto LANGUAGE cpp
    IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/schema PROTOC_OUT_DIR ${proto_gen})
  list(APPEND schema_libs anch-codec-test-pet-proto)
endif()
find_program(ANCH_THRIFT_COMPILER thrift)
if(ANCH_CODEC_THRIFT_ENABLED AND ANCH_THRIFT_COMPILER)
  set(thrift_gen ${CMAKE_CURRENT_BINARY_DIR}/thrift-gen)
  add_custom_command(
    OUTPUT ${thrift_gen}/pet_types.cpp
    BYPRODUCTS ${thrift_gen}/pet_types.h
    COMMAND ${CMAKE_COMMAND} -E make_directory ${thrift_gen}
    COMMAND ${ANCH_THRIFT_COMPILER} --gen cpp -out ${thrift_gen} ${CMAKE_CURRENT_SOURCE_DIR}/schema/pet.thrift
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/schema/pet.thrift
    COMMENT "Generating Thrift test structure")
  add_library(anch-codec-test-pet-thrift STATIC ${thrift_gen}/pet_types.cpp)
  anch_target_build_options(anch-codec-test-pet-thrift)
  target_link_libraries(anch-codec-test-pet-thrift PUBLIC thrift::thrift Boost::headers)
  target_include_directories(anch-codec-test-pet-thrift SYSTEM PUBLIC ${thrift_gen})
  list(APPEND schema_libs anch-codec-test-pet-thrift)
endif()
if(schema_libs AND ANCH_CODEC_JSON_ENABLED)
  anch_add_executable(anch-codec-schema-test SOURCES testSchema.cpp LIBS anch::codec-all ${schema_libs} anch::ut)
  if(ANCH_CODEC_PROTOBUF_ENABLED)
    add_test(NAME anch-codec-protobuf COMMAND anch-codec-schema-test -t protobuf)
  endif()
  if(TARGET anch-codec-test-pet-thrift)
    add_test(NAME anch-codec-thrift COMMAND anch-codec-schema-test -t thrift)
  else()
    # codec/all.hpp includes the Thrift plugin: the test is built without Thrift structure
    target_compile_definitions(anch-codec-schema-test PRIVATE ANCH_CODEC_TEST_NO_THRIFT_COMPILER)
  endif()
endif()
# Plugins with an external schema -
