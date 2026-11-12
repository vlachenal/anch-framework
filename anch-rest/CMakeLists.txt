# AnCH REST: HTTP model, REST API declaration (server), client and transports
macro(anch_rest_sources var dir)
  file(GLOB_RECURSE ${var} CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/src/*.cpp)
endmacro()

# HTTP model and codecs +
anch_rest_sources(common_sources common)
anch_add_library(anch-rest-common
  INCLUDE_DIRS common/inc
  SOURCES ${common_sources}
  PUBLIC_DEPS anch::json)
# HTTP model and codecs -

# REST API declaration and routing +
if(ANCH_REST_SERVER_LIB)
  anch_rest_sources(server_sources server)
  anch_add_library(anch-rest-server
    INCLUDE_DIRS server/inc
    SOURCES ${server_sources}
    PUBLIC_DEPS anch::rest-common anch::uuid)

  # nginx applications bridge (the nginx module itself is built with nginx, see nginx/config)
  if(ANCH_REST_NGINX)
    anch_rest_sources(nginx_sources nginx)
    anch_add_library(anch-rest-nginx
      INCLUDE_DIRS nginx/inc
      SOURCES ${nginx_sources}
      PUBLIC_DEPS anch::rest-server)
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
    find_package(CURL REQUIRED)
    anch_rest_sources(curl_sources curl)
    anch_add_library(anch-rest-curl
      INCLUDE_DIRS curl/inc
      SOURCES ${curl_sources}
      PUBLIC_DEPS anch::rest-client
      PRIVATE_DEPS CURL::libcurl)
  endif()
endif()
# Client and transports -

if(ANCH_BUILD_TESTS)
  add_subdirectory(test)
endif()
