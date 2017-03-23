cmake_minimum_required(VERSION 2.6)

# Project anch date
# ANother C++ Harsh/Huge/Hack
project(anch-date)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include(AnchDate.cmake.in)

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
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  add_definitions("-D ANCH_WINDOWS")
else()
  add_definitions("-D ANCH_POSIX")
endif()
add_definitions(${ANCH_DATE_COMPILE_FLAGS})

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
  anch-date-obj
  OBJECT
  ${source_files}
  )

## Dynamic library +
add_library(
  anch-date
  SHARED
  $<TARGET_OBJECTS:anch-date-obj>
  )

install(
  DIRECTORY inc/ DESTINATION ${ANCH_DATE_INSTALL_INC_DIR}
  FILES_MATCHING PATTERN "*.hpp"
  )

install(
  TARGETS anch-date
  LIBRARY DESTINATION ${ANCH_DATE_INSTALL_LIB_DIR}
  )
## Dynamic library -

## Static library +
if(ANCH_DATE_STATIC_LIB)
  add_library(
    anch-date-static
    STATIC
    $<TARGET_OBJECTS:anch-date-obj>
    )
  set_target_properties(anch-date-static PROPERTIES OUTPUT_NAME anch-date)

  install(
    TARGETS anch-date-static
    ARCHIVE DESTINATION ${ANCH_DATE_INSTALL_LIB_DIR}
    )
endif(ANCH_DATE_STATIC_LIB)
## Static library -

# Unitary tests
if(ANCH_DATE_UNITARY_TEST)

  # Build date format unitary test +
  file(
    GLOB_RECURSE
    test_date_format
    inc/*
    test/testDateFormat.cpp
  )

  add_executable(
    anch-date-format-test
    ${test_date_format}
  )

  if(USE_BOOST_RE)
    link_directories(${Boost_LIBRARY_DIRS})

    target_link_libraries(
      anch-date-format-test
      anch-date
      ${CMAKE_THREAD_LIBS_INIT}
      ${Boost_LIBRARIES}
    )
  else()
    target_link_libraries(
      anch-date-format-test
      anch-date
      ${CMAKE_THREAD_LIBS_INIT}
    )
  endif()
  # Build date format unitary test -

  enable_testing()
  add_test(anch-date-format-test ${EXECUTABLE_OUTPUT_PATH}/anch-date-format-test main)

endif(ANCH_DATE_UNITARY_TEST)
