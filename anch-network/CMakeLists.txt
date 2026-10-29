# AnCH network: TCP/UDP sockets and TCP streams
anch_add_library(anch-network
  PUBLIC_DEPS anch::cutils anch::events)

if(ANCH_BUILD_TESTS)
  add_subdirectory(test)
endif()
