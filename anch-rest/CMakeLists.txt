# AnCH REST: REST router and client
macro(anch_rest_sources var dir)
  file(GLOB_RECURSE ${var} CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/src/*.cpp)
endmacro()

anch_rest_sources(common_sources common)
anch_add_library(anch-rest-common
  INCLUDE_DIRS common/inc
  SOURCES ${common_sources}
  PUBLIC_DEPS anch::json anch::utils)

if(ANCH_REST_SERVER_LIB)
  anch_rest_sources(router_sources router)
  anch_add_library(anch-rest-router
    INCLUDE_DIRS router/inc
    SOURCES ${router_sources}
    PUBLIC_DEPS anch::rest-common anch::uuid)
endif()

if(ANCH_REST_CLIENT_LIB)
  anch_rest_sources(client_sources client)
  anch_add_library(anch-rest-client
    INCLUDE_DIRS client/inc
    SOURCES ${client_sources}
    PUBLIC_DEPS anch::rest-common anch::crypto)
endif()

if(ANCH_BUILD_TESTS)
  add_subdirectory(test)
endif()
