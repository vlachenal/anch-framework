cmake_minimum_required(VERSION 3.25)

# Project anch framework
# ANother C++ Hack
# Build every AnCH Framework components
project(anch-framework
  VERSION 0.1.0
  DESCRIPTION "AnCH Framework - Another C++ Hack"
  HOMEPAGE_URL "https://vlachenal.github.io/anch-framework/"
  LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(AnchHelpers)

# Check compiler +
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 13.0)
  message(FATAL_ERROR "GCC ${CMAKE_CXX_COMPILER_VERSION} < 13.0. AnCH Framework is based on C++23 standard.")
endif()
# Check compiler -

# Options +
# Build
option(BUILD_SHARED_LIBS "Build shared libraries (static libraries otherwise)" ON)
option(ANCH_BUILD_TESTS "Build AnCH unit tests" ${PROJECT_IS_TOP_LEVEL})
option(ANCH_WARNINGS_AS_ERRORS "Treat compilation warnings as errors" ON)
option(ANCH_HARDENING "Enable hardening compilation and link options" ON)
option(ANCH_NATIVE_ARCH "Optimize for the build machine (-march=native)" ON)

# Optional modules
option(ANCH_BUILD_CRYPTO "Build anch-crypto" ON)
option(ANCH_BUILD_SQL "Build anch-sql" ON)
option(ANCH_BUILD_REST "Build anch-rest" OFF)
option(ANCH_BUILD_APP "Build anch-app" OFF)

# Features
option(ANCH_UUID "AnCH UUID support in anch-json" ON)
option(ANCH_DATE "AnCH date support in anch-json (RFC 3339 date-time and full-date)" ON)
option(ANCH_JSON_UTILS "Build anch-json utilities (JSON formatter)" ON)
option(ANCH_CPU_AES "Activate native AES support" OFF)
option(ANCH_CPU_DETECTION "Activate CPU features detection" ON)
option(ANCH_SQL_MYSQL "AnCH MySQL support" ON)
option(ANCH_SQL_POSTGRESQL "AnCH PostgreSQL support" ON)
option(ANCH_SQL_SQLITE3 "AnCH SQLite3 support" ON)
option(ANCH_REST_SERVER_LIB "Build anch-rest server library" ON)
option(ANCH_REST_CLIENT_LIB "Build anch-rest client library" ON)
# Options -

# Global settings +
# Default to optimized build for single configuration generators
get_property(multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(NOT multi_config AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

if(WIN32)
  set(ANCH_PLATFORM_DEFINITION ANCH_WINDOWS)
else()
  set(ANCH_PLATFORM_DEFINITION ANCH_POSIX)
endif()

find_package(Threads REQUIRED)

if(ANCH_BUILD_TESTS)
  enable_testing()
endif()
# Global settings -

# Modules (in dependency order) +
add_subdirectory(anch-utils)
add_subdirectory(anch-cli)
add_subdirectory(anch-events)
add_subdirectory(anch-cutils)
add_subdirectory(anch-network)
add_subdirectory(anch-ut)
add_subdirectory(anch-uuid)
add_subdirectory(anch-date)
add_subdirectory(anch-ini)
add_subdirectory(anch-conf)
add_subdirectory(anch-log)
add_subdirectory(anch-device)
add_subdirectory(anch-json)
if(ANCH_BUILD_CRYPTO)
  add_subdirectory(anch-crypto)
endif()
if(ANCH_BUILD_SQL)
  add_subdirectory(anch-sql)
endif()
if(ANCH_BUILD_REST)
  add_subdirectory(anch-rest)
endif()
if(ANCH_BUILD_APP)
  add_subdirectory(anch-app)
endif()
# Modules -

# Package export +
include(CMakePackageConfigHelpers)
set(ANCH_CMAKE_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/Anch)

install(EXPORT AnchTargets
  NAMESPACE anch::
  DESTINATION ${ANCH_CMAKE_INSTALL_DIR})

configure_package_config_file(cmake/AnchConfig.cmake.in
  ${PROJECT_BINARY_DIR}/AnchConfig.cmake
  INSTALL_DESTINATION ${ANCH_CMAKE_INSTALL_DIR})
write_basic_package_version_file(${PROJECT_BINARY_DIR}/AnchConfigVersion.cmake
  COMPATIBILITY SameMinorVersion)

install(FILES
  ${PROJECT_BINARY_DIR}/AnchConfig.cmake
  ${PROJECT_BINARY_DIR}/AnchConfigVersion.cmake
  DESTINATION ${ANCH_CMAKE_INSTALL_DIR})
# Package export -
