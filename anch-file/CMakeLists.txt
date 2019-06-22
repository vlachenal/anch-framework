cmake_minimum_required(VERSION 2.6)

# Project anch file
# ANother C++ Harsh/Huge/Hack
project(anch-file)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include(AnchFile.cmake.in)

# Check GCC version +
execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                OUTPUT_VARIABLE GCC_VERSION)
if (GCC_VERSION VERSION_LESS 4.5)
  message(FATAL_ERROR "Version < 4.5. This library is based on C++11 standard. Exit.")
elseif(GCC_VERSION VERSION_GREATER 8.0 OR GCC_VERSION VERSION_EQUAL 8.0)
  message(STATUS "Version >= 8.0 use -std=c++17")
  add_definitions("-std=c++17")
  add_definitions("-D ANCH_TM_SPEC")
elseif(GCC_VERSION VERSION_GREATER 6.0 OR GCC_VERSION VERSION_EQUAL 6.0)
  message(STATUS "Version >= 6.0 use -std=c++14")
  add_definitions("-std=c++14")
elseif(GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7)
  message(STATUS "Version >= 4.7 use -std=c++11")
  add_definitions("-std=c++11")
else()
  message(STATUS "4.5 <= Version < 4.7 use -std=c++0x")
  add_definitions("-std=c++0x")
endif()
# Check GCC version -

# Add GCC definitions
add_definitions("-Wall -Wextra -Werror -Wuninitialized -Wconversion -Wsign-conversion -Wformat-security -fstack-protector-all -Wstack-protector --param ssp-buffer-size=4 -ftrapv  -z relro,now -fPIC -D_FORTIFY_SOURCE=2")
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  add_definitions("-D ANCH_WINDOWS")
else()
  add_definitions("-D ANCH_POSIX")
endif()
add_definitions(${ANCH_FILE_COMPILE_FLAGS})

# Include librairies
include(FindThreads)
find_package(Threads)

# Includes location
include_directories(
  inc
  ${ANCH_DATE_INCLUDES_DIR}
)

# Retrieve every files in project
file(
  GLOB_RECURSE
  source_files
  src/*
)

# Build objects
add_library(
  anch-file-obj
  OBJECT
  ${source_files}
  )

## Dynamic library +
add_library(
  anch-file
  SHARED
  $<TARGET_OBJECTS:anch-file-obj>
  )

install(
  DIRECTORY inc/ DESTINATION ${ANCH_FILE_INSTALL_INC_DIR}
  FILES_MATCHING PATTERN "*.hpp"
  )

install(
  TARGETS anch-file
  LIBRARY DESTINATION ${ANCH_FILE_INSTALL_LIB_DIR}
  )
## Dynamic library -

## Static library +
if(ANCH_FILE_STATIC_LIB)
  add_library(
    anch-file-static
    STATIC
    $<TARGET_OBJECTS:anch-file-obj>
    )
  set_target_properties(anch-file-static PROPERTIES OUTPUT_NAME anch-file)

  install(
    TARGETS anch-file-static
    ARCHIVE DESTINATION ${ANCH_FILE_INSTALL_LIB_DIR}
    )
endif(ANCH_FILE_STATIC_LIB)
## Static library -

if(ANCH_FILE_UNITARY_TEST)

  find_package(Boost COMPONENTS regex)

  link_directories(
    ${Boost_LIBRARY_DIRS}
    ${ANCH_DATE_LIBRARY_DIR}
  )

  # Build parser unitary test +
  file(
    GLOB_RECURSE
    test_files
    test/testFile.cpp
  )

  add_executable(
    anch-file-test
    ${test_files}
  )

  target_link_libraries(
    anch-file-test
    anch-file
    anch-date
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES}
  )
  # Build parser unitary test -

  enable_testing()
  add_test(anch-file-test ${EXECUTABLE_OUTPUT_PATH}/anch-file-test main)

endif()
