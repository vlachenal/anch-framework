set_property(DIRECTORY PROPERTY LABELS rest)

if(ANCH_REST_SERVER_LIB)
  anch_add_example(rest-server SOURCES server.cpp LIBS anch::rest-server RUN)
endif()
if(ANCH_REST_CLIENT_LIB AND ANCH_REST_CURL_ENABLED)
  anch_add_example(rest-client SOURCES client.cpp LIBS anch::rest-client anch::rest-curl)
endif()

# nginx application: shared library loaded by the anch-rest nginx module (built, not run) +
if(ANCH_REST_SERVER_LIB AND ANCH_REST_NGINX)
  add_library(anch-example-rest-nginx-app MODULE nginxApp.cpp)
  anch_target_build_options(anch-example-rest-nginx-app)
  target_link_libraries(anch-example-rest-nginx-app PRIVATE anch::rest-nginx)
  # Out of lib/: cmake/check-exports.sh checks framework libraries only
  set_target_properties(anch-example-rest-nginx-app PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    CXX_VISIBILITY_PRESET hidden)
endif()
# nginx application: shared library loaded by the anch-rest nginx module (built, not run) -

# Protocol Buffers and gRPC: pets.proto (package pets, as in documentation) +
if(TARGET anch::rest-protobuf AND ANCH_REST_SERVER_LIB AND ANCH_REST_CLIENT_LIB)
  set(rpc_gen ${CMAKE_CURRENT_BINARY_DIR}/rpc-gen)
  file(MAKE_DIRECTORY ${rpc_gen})
  add_library(anch-example-pets-proto STATIC pets.proto)
  anch_target_build_options(anch-example-pets-proto)
  target_link_libraries(anch-example-pets-proto PUBLIC anch::rest-protobuf)
  target_include_directories(anch-example-pets-proto SYSTEM PUBLIC ${rpc_gen})
  protobuf_generate(TARGET anch-example-pets-proto LANGUAGE cpp
    IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR} PROTOC_OUT_DIR ${rpc_gen})
  anch_add_example(rest-protobuf SOURCES protobuf.cpp
    LIBS anch::rest-protobuf anch::rest-server anch::rest-client anch-example-pets-proto RUN)
  if(TARGET anch::rest-grpc)
    anch_add_example(rest-grpc SOURCES grpc.cpp LIBS anch::rest-grpc anch-example-pets-proto)
  endif()
endif()
# Protocol Buffers and gRPC: pets.proto (package pets, as in documentation) -

# Apache Thrift: pets.thrift (namespace pets, as in documentation) +
find_program(ANCH_THRIFT_COMPILER thrift)
if(TARGET anch::rest-thrift AND ANCH_REST_SERVER_LIB AND ANCH_REST_CLIENT_LIB AND ANCH_THRIFT_COMPILER)
  set(thrift_gen ${CMAKE_CURRENT_BINARY_DIR}/thrift-gen)
  set(thrift_sources ${thrift_gen}/pets_types.cpp ${thrift_gen}/PetStore.cpp)
  add_custom_command(
    OUTPUT ${thrift_sources}
    BYPRODUCTS ${thrift_gen}/pets_types.h ${thrift_gen}/PetStore.h ${thrift_gen}/PetStore_server.skeleton.cpp
    COMMAND ${CMAKE_COMMAND} -E make_directory ${thrift_gen}
    COMMAND ${ANCH_THRIFT_COMPILER} --gen cpp -out ${thrift_gen} ${CMAKE_CURRENT_SOURCE_DIR}/pets.thrift
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pets.thrift)
  add_library(anch-example-pets-thrift STATIC ${thrift_sources})
  anch_target_build_options(anch-example-pets-thrift)
  target_link_libraries(anch-example-pets-thrift PUBLIC thrift::thrift Boost::headers)
  target_include_directories(anch-example-pets-thrift SYSTEM PUBLIC ${thrift_gen})
  anch_add_example(rest-thrift SOURCES thrift.cpp
    LIBS anch::rest-thrift anch::rest-server anch::rest-client anch-example-pets-thrift RUN)
endif()
# Apache Thrift: pets.thrift (namespace pets, as in documentation) -
