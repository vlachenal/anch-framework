# AnCH network: sockets, TCP streams and UDP datagrams
anch_add_library(anch-network
  PUBLIC_DEPS anch::cutils)

if(ANCH_BUILD_TESTS)
  add_subdirectory(test)
endif()
if(ANCH_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
