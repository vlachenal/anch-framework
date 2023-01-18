cmake_minimum_required(VERSION 3.0)

# Project anch framework
# ANother C++ Hack
# This is a meta-build to build every ANCH Framework components
project(anch-framework)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})

add_subdirectory(anch-resource)
add_subdirectory(anch-date)
add_subdirectory(anch-logger)
add_subdirectory(anch-events)
add_subdirectory(anch-uuid)
add_subdirectory(anch-network)
add_subdirectory(anch-device)
add_subdirectory(anch-crypto)
add_subdirectory(anch-sql)
add_subdirectory(anch-json)
add_subdirectory(anch-rest)
add_subdirectory(anch-utils)
