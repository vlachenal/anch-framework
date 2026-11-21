# AnCH REST: HTTP model, REST API declaration (server), client and transports (codecs are in anch-codec)
macro(anch_rest_sources var dir)
  file(GLOB_RECURSE ${var} CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/src/*.cpp)
endmacro()

# HTTP model and codecs +
anch_rest_sources(common_sources common)
anch_add_library(anch-rest-common
  INCLUDE_DIRS common/inc
  SOURCES ${common_sources}
  PUBLIC_DEPS anch::codec-json anch::uuid)
# HTTP model and codecs -

# REST API declaration and routing +
if(ANCH_REST_SERVER_LIB)
  anch_rest_sources(server_sources server)
  anch_add_library(anch-rest-server
    INCLUDE_DIRS server/inc
    SOURCES ${server_sources}
    PUBLIC_DEPS anch::rest-common)

  # nginx applications bridge (the nginx module itself is built with nginx, see nginx/config)
  if(ANCH_REST_NGINX)
    anch_rest_sources(nginx_sources nginx)
    anch_add_library(anch-rest-nginx
      INCLUDE_DIRS nginx/inc
      SOURCES ${nginx_sources}
      PUBLIC_DEPS anch::rest-server
      PRIVATE_DEPS anch::cutils) # request and response bodies in stream
    install(FILES nginx/config nginx/ngx_http_anch_rest_module.c
      DESTINATION ${CMAKE_INSTALL_DATADIR}/anch/nginx-module)
    install(FILES nginx/inc/rest/anch_rest_nginx.h
      DESTINATION ${CMAKE_INSTALL_DATADIR}/anch/nginx-module/inc/rest)
  endif()
endif()
# REST API declaration and routing -

# Client and transports +
if(ANCH_REST_CLIENT_LIB)
  anch_rest_sources(client_sources client)
  anch_add_library(anch-rest-client
    INCLUDE_DIRS client/inc
    SOURCES ${client_sources}
    PUBLIC_DEPS anch::rest-common)

  if(ANCH_REST_CURL)
    find_package(CURL QUIET)
    if(CURL_FOUND)
      set(ANCH_REST_CURL_ENABLED ON)
      anch_rest_sources(curl_sources curl)
      anch_add_library(anch-rest-curl
        INCLUDE_DIRS curl/inc
        SOURCES ${curl_sources}
        PUBLIC_DEPS anch::rest-client
        PRIVATE_DEPS CURL::libcurl)
    else()
      message(STATUS "libcurl not found: anch-rest-curl is not built")
    endif()
  endif()
endif()
set(ANCH_REST_CURL_ENABLED ${ANCH_REST_CURL_ENABLED} PARENT_SCOPE)
# Client and transports -

# Protocol Buffers RPC (Connect / gRPC-Web) +
set(ANCH_REST_PROTOBUF_ENABLED OFF)
if(ANCH_REST_PROTOBUF AND TARGET anch::codec-protobuf)
  set(ANCH_REST_PROTOBUF_ENABLED ON)
  anch_rest_sources(protobuf_sources protobuf)
  anch_add_library(anch-rest-protobuf
    INCLUDE_DIRS protobuf/inc
    SOURCES ${protobuf_sources}
    PUBLIC_DEPS anch::rest-common anch::codec-protobuf)
elseif(ANCH_REST_PROTOBUF)
  message(STATUS "anch-codec-protobuf is not built: anch-rest-protobuf is not built")
endif()
set(ANCH_REST_PROTOBUF_ENABLED ${ANCH_REST_PROTOBUF_ENABLED} PARENT_SCOPE)
# Protocol Buffers RPC (Connect / gRPC-Web) -

# gRPC server +
set(ANCH_REST_GRPC_ENABLED OFF)
if(ANCH_REST_GRPC AND ANCH_REST_PROTOBUF_ENABLED)
  find_package(gRPC CONFIG QUIET)
  if(gRPC_FOUND)
    set(ANCH_REST_GRPC_ENABLED ON)
    anch_rest_sources(grpc_sources grpc)
    anch_add_library(anch-rest-grpc
      INCLUDE_DIRS grpc/inc
      SOURCES ${grpc_sources}
      PUBLIC_DEPS anch::rest-protobuf gRPC::grpc++)
  else()
    message(STATUS "gRPC not found: anch-rest-grpc is not built")
  endif()
elseif(ANCH_REST_GRPC)
  message(STATUS "anch-rest-protobuf is not built: anch-rest-grpc is not built")
endif()
set(ANCH_REST_GRPC_ENABLED ${ANCH_REST_GRPC_ENABLED} PARENT_SCOPE)
# gRPC server -

# Apache Thrift RPC +
set(ANCH_REST_THRIFT_ENABLED OFF)
if(ANCH_REST_THRIFT AND TARGET anch::codec-thrift)
  set(ANCH_REST_THRIFT_ENABLED ON)
  # Header only: the codecs are in anch-codec-thrift
  anch_add_library(anch-rest-thrift HEADER_ONLY
    INCLUDE_DIRS thrift/inc
    PUBLIC_DEPS anch::rest-common anch::codec-thrift)
elseif(ANCH_REST_THRIFT)
  message(STATUS "anch-codec-thrift is not built: anch-rest-thrift is not built")
endif()
set(ANCH_REST_THRIFT_ENABLED ${ANCH_REST_THRIFT_ENABLED} PARENT_SCOPE)
# Apache Thrift RPC -

if(ANCH_BUILD_TESTS)
  add_subdirectory(test)
endif()
if(ANCH_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
