# AnCH REST: HTTP model, REST API declaration (server) and client
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
endif()
# REST API declaration and routing -

# Client is being rewritten (see anch-rest.todo): client/, curl/ and nginx/ are not built
if(ANCH_REST_CLIENT_LIB)
  message(WARNING "anch-rest client is being rewritten: it is not built")
endif()

if(ANCH_BUILD_TESTS)
  add_subdirectory(test)
endif()
