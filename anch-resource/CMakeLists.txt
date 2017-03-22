cmake_minimum_required(VERSION 2.8.8)

# Project anch resource
# ANother C++ Harsh/Huge/Hack
project(anch-resource)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include(AnchResource.cmake.in)

# Check GCC version +
execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                OUTPUT_VARIABLE GCC_VERSION)
if (GCC_VERSION VERSION_LESS 4.5)
  message(FATAL_ERROR "Version < 4.5. This library is based on C++11 standard. Exit.")
elseif(GCC_VERSION VERSION_GREATER 6.0 OR GCC_VERSION VERSION_EQUAL 6.0)
  message(STATUS "Version >= 6.0 use -std=c++14")
  add_definitions("-std=c++14")
elseif(GCC_VERSION VERSION_GREATER 4.9 OR GCC_VERSION VERSION_EQUAL 4.9)
  message(STATUS "Version >= 4.9 use -std=c++11")
  add_definitions("-std=c++11")
elseif(GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7)
  message(STATUS "Version >= 4.7 and < 4.9 use -std=c++11 with boost regular expression")
  add_definitions("-std=c++11")
  add_definitions("-D ANCH_BOOST_REGEX")
  set(USE_BOOST_RE true)
else()
  message(STATUS "4.5 <= Version < 4.7 use -std=c++0x")
  add_definitions("-std=c++0x")
  add_defintions("-D ANCH_BOOST_REGEX")
  set(USE_BOOST_RE true)
endif()
# Check GCC version -

# Add GCC definitions
add_definitions("-Wall -Wextra -Werror -Wuninitialized -Wconversion -Wsign-conversion -Wformat-security -fstack-protector-all -Wstack-protector --param ssp-buffer-size=4 -ftrapv  -z relro,now -fPIC -D_FORTIFY_SOURCE=2")
add_definitions(${ANCH_RESOURCE_COMPILE_FLAGS})

# Include librairies
include(FindThreads)
find_package(Threads)

# Includes location
include_directories(inc)
if(USE_BOOST_RE)
  find_package(Boost COMPONENTS regex)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

# Retrieve every files in project
file(
  GLOB_RECURSE
  source_files
  src/*
  inc/*
)

# Build objects
add_library(
  anch-resource-obj
  OBJECT
  ${source_files}
  )

## Dynamic library +
add_library(
  anch-resource
  SHARED
  $<TARGET_OBJECTS:anch-resource-obj>
  )

install(
  DIRECTORY inc/ DESTINATION ${ANCH_RESOURCE_INSTALL_INC_DIR}
  FILES_MATCHING PATTERN "*.hpp"
  )

install(
  TARGETS anch-resource
  LIBRARY DESTINATION ${ANCH_RESOURCE_INSTALL_LIB_DIR}
  )
## Dynamic library -

## Static library +
if(ANCH_RESOURCE_STATIC_LIB)
  add_library(
    anch-resource-static
    STATIC
    $<TARGET_OBJECTS:anch-resource-obj>
    )
  set_target_properties(anch-resource-static PROPERTIES OUTPUT_NAME anch-resource)

  install(
    TARGETS anch-resource-static
    ARCHIVE DESTINATION ${ANCH_RESOURCE_INSTALL_LIB_DIR}
    )
endif(ANCH_RESOURCE_STATIC_LIB)
## Static library -


if(ANCH_RESOURCE_UNITARY_TEST)

  # Build parser unitary test +
  file(
    GLOB_RECURSE
    test_files
    inc/*
    test/testResource.cpp
  )

  add_executable(
    anch-resource-parse
    ${test_files}
  )

  if(USE_BOOST_RE)
    link_directories(${Boost_LIBRARY_DIRS})

    target_link_libraries(
      anch-resource-parse
      anch-resource
      ${CMAKE_THREAD_LIBS_INIT}
      ${Boost_LIBRARIES}
    )
  else()
    target_link_libraries(
      anch-resource-parse
      anch-resource
      ${CMAKE_THREAD_LIBS_INIT}
    )
  endif()
  # Build parser unitary test -

  enable_testing()
  add_test(anch-resource-parse ${EXECUTABLE_OUTPUT_PATH}/anch-resource-parse main)

endif(ANCH_RESOURCE_UNITARY_TEST)
