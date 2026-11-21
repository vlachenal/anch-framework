set_property(DIRECTORY PROPERTY LABELS rest)

anch_add_executable(anch-rest-http-test SOURCES testHttp.cpp LIBS anch::rest-common anch::ut)

add_test(NAME anch-rest-http-headers COMMAND anch-rest-http-test -t headers)
add_test(NAME anch-rest-http-method-status COMMAND anch-rest-http-test -t method-status)
add_test(NAME anch-rest-http-request-target COMMAND anch-rest-http-test -t request-target)
add_test(NAME anch-rest-http-response COMMAND anch-rest-http-test -t response)
add_test(NAME anch-rest-http-form COMMAND anch-rest-http-test -t form)
add_test(NAME anch-rest-http-form-url-encoded COMMAND anch-rest-http-test -t form-url-encoded)
add_test(NAME anch-rest-http-multipart COMMAND anch-rest-http-test -t multipart)
add_test(NAME anch-rest-http-multipart-malformed COMMAND anch-rest-http-test -t multipart-malformed)
add_test(NAME anch-rest-http-multipart-limits COMMAND anch-rest-http-test -t multipart-limits)
add_test(NAME anch-rest-http-part-file COMMAND anch-rest-http-test -t part-file)
add_test(NAME anch-rest-http-multipart-stream COMMAND anch-rest-http-test -t multipart-stream)
add_test(NAME anch-rest-http-multipart-stream-write COMMAND anch-rest-http-test -t multipart-stream-write)

# Deprecated codec names (moved to anch-codec, removed in 0.4) +
anch_add_executable(anch-rest-deprecated-test SOURCES testDeprecated.cpp LIBS anch::rest-common anch::ut)
target_compile_options(anch-rest-deprecated-test PRIVATE -Wno-deprecated-declarations)
add_test(NAME anch-rest-deprecated-aliases COMMAND anch-rest-deprecated-test -t aliases)

# Uses of deprecated names are reported: compiled by the test, outside of the build
add_library(anch-rest-deprecated-warnings OBJECT EXCLUDE_FROM_ALL testDeprecated.cpp)
anch_target_build_options(anch-rest-deprecated-warnings)
target_link_libraries(anch-rest-deprecated-warnings PRIVATE anch::rest-common anch::ut)
target_compile_options(anch-rest-deprecated-warnings PRIVATE -Werror=deprecated-declarations)
add_test(NAME anch-rest-deprecated-warnings
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target anch-rest-deprecated-warnings)
set_tests_properties(anch-rest-deprecated-warnings PROPERTIES
  PASS_REGULAR_EXPRESSION "deprecated-declarations")
# Deprecated codec names -

if(ANCH_REST_SERVER_LIB)
  anch_add_executable(anch-rest-api-test SOURCES testApi.cpp LIBS anch::rest-server anch::ut)

  add_test(NAME anch-rest-api-routing COMMAND anch-rest-api-test -t routing)
  add_test(NAME anch-rest-api-method-not-allowed COMMAND anch-rest-api-test -t method-not-allowed)
  add_test(NAME anch-rest-api-head COMMAND anch-rest-api-test -t head)
  add_test(NAME anch-rest-api-options COMMAND anch-rest-api-test -t options)
  add_test(NAME anch-rest-api-path-parameters COMMAND anch-rest-api-test -t path-parameters)
  add_test(NAME anch-rest-api-results COMMAND anch-rest-api-test -t results)
  add_test(NAME anch-rest-api-negotiation COMMAND anch-rest-api-test -t negotiation)
  add_test(NAME anch-rest-api-body COMMAND anch-rest-api-test -t body)
  add_test(NAME anch-rest-api-constraints COMMAND anch-rest-api-test -t constraints)
  add_test(NAME anch-rest-api-status-and-query COMMAND anch-rest-api-test -t status-and-query)
  add_test(NAME anch-rest-api-filters COMMAND anch-rest-api-test -t filters)
  add_test(NAME anch-rest-api-cors COMMAND anch-rest-api-test -t cors)
  add_test(NAME anch-rest-api-codec-media-type COMMAND anch-rest-api-test -t codec-media-type)
  add_test(NAME anch-rest-api-form COMMAND anch-rest-api-test -t form)
  add_test(NAME anch-rest-api-multipart COMMAND anch-rest-api-test -t multipart)
  add_test(NAME anch-rest-api-form-mapper COMMAND anch-rest-api-test -t form-mapper)
  add_test(NAME anch-rest-api-raw-body COMMAND anch-rest-api-test -t raw-body)
  add_test(NAME anch-rest-api-errors COMMAND anch-rest-api-test -t errors)
  add_test(NAME anch-rest-api-registration COMMAND anch-rest-api-test -t registration)
  add_test(NAME anch-rest-api-validate COMMAND anch-rest-api-test -t validate)
endif()

if(ANCH_REST_CLIENT_LIB)
  anch_add_executable(anch-rest-client-test SOURCES testClient.cpp LIBS anch::rest-client anch::ut)

  add_test(NAME anch-rest-client-format-path COMMAND anch-rest-client-test -t format-path)
  add_test(NAME anch-rest-client-url COMMAND anch-rest-client-test -t url)
  add_test(NAME anch-rest-client-headers COMMAND anch-rest-client-test -t headers)
  add_test(NAME anch-rest-client-body COMMAND anch-rest-client-test -t body)
  add_test(NAME anch-rest-client-as COMMAND anch-rest-client-test -t as)
  add_test(NAME anch-rest-client-codec-media-type COMMAND anch-rest-client-test -t codec-media-type)
  add_test(NAME anch-rest-client-form COMMAND anch-rest-client-test -t form)
  add_test(NAME anch-rest-client-multipart COMMAND anch-rest-client-test -t multipart)
  add_test(NAME anch-rest-client-form-mapper COMMAND anch-rest-client-test -t form-mapper)
  add_test(NAME anch-rest-client-multipart-stream COMMAND anch-rest-client-test -t multipart-stream)
  add_test(NAME anch-rest-client-raw-body COMMAND anch-rest-client-test -t raw-body)
  add_test(NAME anch-rest-client-constraints COMMAND anch-rest-client-test -t constraints)
  add_test(NAME anch-rest-client-http-error COMMAND anch-rest-client-test -t http-error)
  add_test(NAME anch-rest-client-filters COMMAND anch-rest-client-test -t filters)
  add_test(NAME anch-rest-client-base64 COMMAND anch-rest-client-test -t base64)
endif()

if(ANCH_REST_CLIENT_LIB AND ANCH_REST_CURL_ENABLED AND ANCH_REST_SERVER_LIB)
  anch_add_executable(anch-rest-curl-test SOURCES testCurl.cpp LIBS anch::rest-curl anch::rest-server anch::network anch::ut)

  add_test(NAME anch-rest-curl-get-json COMMAND anch-rest-curl-test -t get-json)
  add_test(NAME anch-rest-curl-parameters COMMAND anch-rest-curl-test -t parameters)
  add_test(NAME anch-rest-curl-post-json COMMAND anch-rest-curl-test -t post-json)
  add_test(NAME anch-rest-curl-multipart COMMAND anch-rest-curl-test -t multipart)
  add_test(NAME anch-rest-curl-multipart-large COMMAND anch-rest-curl-test -t multipart-large)
  add_test(NAME anch-rest-curl-constraints COMMAND anch-rest-curl-test -t constraints)
  add_test(NAME anch-rest-curl-http-error COMMAND anch-rest-curl-test -t http-error)
  add_test(NAME anch-rest-curl-head COMMAND anch-rest-curl-test -t head)
  add_test(NAME anch-rest-curl-auth COMMAND anch-rest-curl-test -t auth)
  add_test(NAME anch-rest-curl-redirect-stream COMMAND anch-rest-curl-test -t redirect-stream)
  add_test(NAME anch-rest-curl-invalid-header COMMAND anch-rest-curl-test -t invalid-header)
  add_test(NAME anch-rest-curl-response-too-large COMMAND anch-rest-curl-test -t response-too-large)
  add_test(NAME anch-rest-curl-refused COMMAND anch-rest-curl-test -t refused)
  add_test(NAME anch-rest-curl-timeout COMMAND anch-rest-curl-test -t timeout)
  # Tests share a local HTTP server on a free port: the server is started for each test executable
endif()

if(ANCH_REST_SERVER_LIB AND ANCH_REST_NGINX)
  # Test application, also used by nginx/test/run-nginx-test.sh
  add_library(anch-rest-nginx-test-app MODULE ../nginx/test/petApp.cpp)
  anch_target_build_options(anch-rest-nginx-test-app)
  target_link_libraries(anch-rest-nginx-test-app PRIVATE anch::rest-nginx)
  set_target_properties(anch-rest-nginx-test-app PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    CXX_VISIBILITY_PRESET hidden)

  anch_add_executable(anch-rest-nginx-test SOURCES testNginx.cpp LIBS anch::rest-nginx anch::ut ${CMAKE_DL_LIBS})
  target_compile_definitions(anch-rest-nginx-test PRIVATE ANCH_REST_NGINX_TEST_APP="$<TARGET_FILE:anch-rest-nginx-test-app>")
  add_dependencies(anch-rest-nginx-test anch-rest-nginx-test-app)

  add_test(NAME anch-rest-nginx-create-error COMMAND anch-rest-nginx-test -t create-error)
  add_test(NAME anch-rest-nginx-get COMMAND anch-rest-nginx-test -t get)
  add_test(NAME anch-rest-nginx-post COMMAND anch-rest-nginx-test -t post)
  add_test(NAME anch-rest-nginx-request COMMAND anch-rest-nginx-test -t request)
  add_test(NAME anch-rest-nginx-errors COMMAND anch-rest-nginx-test -t errors)
  add_test(NAME anch-rest-nginx-cors COMMAND anch-rest-nginx-test -t cors)
  add_test(NAME anch-rest-nginx-stream-small COMMAND anch-rest-nginx-test -t stream-small)
  add_test(NAME anch-rest-nginx-stream-large COMMAND anch-rest-nginx-test -t stream-large)
  add_test(NAME anch-rest-nginx-echo COMMAND anch-rest-nginx-test -t echo)
  add_test(NAME anch-rest-nginx-read-error COMMAND anch-rest-nginx-test -t read-error)
  add_test(NAME anch-rest-nginx-write-cancelled COMMAND anch-rest-nginx-test -t write-cancelled)
  add_test(NAME anch-rest-nginx-crash COMMAND anch-rest-nginx-test -t crash)
endif()

# Protocol Buffers and gRPC +
if(TARGET anch::rest-protobuf AND ANCH_REST_SERVER_LIB AND ANCH_REST_CLIENT_LIB)
  # Test messages and services generated by protoc (and grpc_cpp_plugin)
  set(rpc_gen ${CMAKE_CURRENT_BINARY_DIR}/rpc-gen)
  file(MAKE_DIRECTORY ${rpc_gen})
  add_library(anch-rest-test-pets-proto STATIC rpc/pets.proto)
  anch_target_build_options(anch-rest-test-pets-proto)
  target_link_libraries(anch-rest-test-pets-proto PUBLIC anch::rest-protobuf)
  target_include_directories(anch-rest-test-pets-proto SYSTEM PUBLIC ${rpc_gen})
  protobuf_generate(TARGET anch-rest-test-pets-proto LANGUAGE cpp
    IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/rpc PROTOC_OUT_DIR ${rpc_gen})

  anch_add_executable(anch-rest-protobuf-test SOURCES testProtobuf.cpp
    LIBS anch::rest-protobuf anch::rest-server anch::rest-client anch-rest-test-pets-proto anch::ut)
  target_include_directories(anch-rest-protobuf-test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  if(ANCH_REST_CURL_ENABLED)
    target_link_libraries(anch-rest-protobuf-test PRIVATE anch::rest-curl anch::network)
    target_compile_definitions(anch-rest-protobuf-test PRIVATE ANCH_REST_TEST_CURL)
    add_test(NAME anch-rest-protobuf-curl COMMAND anch-rest-protobuf-test -t curl)
  endif()
  add_test(NAME anch-rest-protobuf-codecs COMMAND anch-rest-protobuf-test -t codecs)
  add_test(NAME anch-rest-protobuf-connect COMMAND anch-rest-protobuf-test -t connect)
  add_test(NAME anch-rest-protobuf-grpc-web COMMAND anch-rest-protobuf-test -t grpc-web)
  add_test(NAME anch-rest-protobuf-client COMMAND anch-rest-protobuf-test -t client)

  if(TARGET anch::rest-grpc)
    add_library(anch-rest-test-pets-grpc STATIC rpc/pets.proto)
    anch_target_build_options(anch-rest-test-pets-grpc)
    target_link_libraries(anch-rest-test-pets-grpc PUBLIC anch-rest-test-pets-proto gRPC::grpc++)
    protobuf_generate(TARGET anch-rest-test-pets-grpc LANGUAGE grpc
      GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
      PLUGIN "protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>"
      IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/rpc PROTOC_OUT_DIR ${rpc_gen})

    anch_add_executable(anch-rest-grpc-test SOURCES testGrpc.cpp
      LIBS anch::rest-grpc anch::rest-server anch-rest-test-pets-grpc anch::ut)
    target_include_directories(anch-rest-grpc-test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    add_test(NAME anch-rest-grpc-calls COMMAND anch-rest-grpc-test -t calls)
    add_test(NAME anch-rest-grpc-shared-service COMMAND anch-rest-grpc-test -t shared-service)
  endif()
endif()
# Protocol Buffers and gRPC -

# Apache Thrift +
find_program(ANCH_THRIFT_COMPILER thrift)
if(TARGET anch::rest-thrift AND ANCH_REST_SERVER_LIB AND ANCH_REST_CLIENT_LIB AND ANCH_THRIFT_COMPILER)
  # Test structures and service generated by thrift
  set(thrift_gen ${CMAKE_CURRENT_BINARY_DIR}/thrift-gen)
  set(thrift_sources ${thrift_gen}/pets_types.cpp ${thrift_gen}/PetStore.cpp)
  add_custom_command(
    OUTPUT ${thrift_sources}
    BYPRODUCTS ${thrift_gen}/pets_types.h ${thrift_gen}/PetStore.h ${thrift_gen}/PetStore_server.skeleton.cpp
    COMMAND ${CMAKE_COMMAND} -E make_directory ${thrift_gen}
    COMMAND ${ANCH_THRIFT_COMPILER} --gen cpp -out ${thrift_gen} ${CMAKE_CURRENT_SOURCE_DIR}/rpc/pets.thrift
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpc/pets.thrift
    COMMENT "Generating Thrift pet store")
  add_library(anch-rest-test-pets-thrift STATIC ${thrift_sources})
  anch_target_build_options(anch-rest-test-pets-thrift)
  target_link_libraries(anch-rest-test-pets-thrift PUBLIC thrift::thrift Boost::headers)
  target_include_directories(anch-rest-test-pets-thrift SYSTEM PUBLIC ${thrift_gen})

  anch_add_executable(anch-rest-thrift-test SOURCES testThrift.cpp
    LIBS anch::rest-thrift anch::rest-server anch::rest-client anch-rest-test-pets-thrift anch::ut)
  target_include_directories(anch-rest-thrift-test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  if(ANCH_REST_CURL_ENABLED)
    target_link_libraries(anch-rest-thrift-test PRIVATE anch::rest-curl anch::network)
    target_compile_definitions(anch-rest-thrift-test PRIVATE ANCH_REST_TEST_CURL)
    add_test(NAME anch-rest-thrift-curl COMMAND anch-rest-thrift-test -t curl)
  endif()
  add_test(NAME anch-rest-thrift-codecs COMMAND anch-rest-thrift-test -t codecs)
  add_test(NAME anch-rest-thrift-rpc COMMAND anch-rest-thrift-test -t rpc)
endif()
# Apache Thrift -
