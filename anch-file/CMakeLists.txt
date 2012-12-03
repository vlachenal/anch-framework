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
elseif(GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7)
  message(STATUS "Version >= 4.7 use -std=c++11")
  add_definitions("-std=c++11")
else()
  message(STATUS "4.5 <= Version < 4.7 use -std=c++0x")
  add_definitions("-std=c++0x")
endif()
# Check GCC version -

# Add GCC definitions
add_definitions("-Wall -Werror")
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
  inc/*
)

# Declare exec
add_library(
  anch-file
  SHARED
  ${source_files}
)

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
    inc/*
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
