cmake_minimum_required(VERSION 2.6)

# Project anch logger
# ANother C++ Hack
project(anch-logger)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include(AnchLogger.cmake.in)

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
add_definitions("-Wall -Wextra -Werror")
add_definitions(${ANCH_LOGGER_COMPILE_FLAGS})

# Include librairies
include(FindThreads)
find_package(Threads)

# Includes location
include_directories(
  inc
  ${ANCH_RESOURCE_INCLUDES_DIR}
  ${ANCH_DATE_INCLUDES_DIR}
  ${ANCH_UTILS_INCLUDES_DIR}
)

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

# Declare exec
add_library(
  anch-logger
  SHARED
  ${source_files}
)


# Unitary tests
if(ANCH_LOGGER_UNITARY_TEST)

  if(USE_BOOST_RE)
    link_directories(
      ${Boost_LIBRARY_DIRS}
      ${ANCH_RESOURCE_LIBRARY_DIR}
      ${ANCH_DATE_LIBRARY_DIR}
    )
  else()
    link_directories(
      ${ANCH_RESOURCE_LIBRARY_DIR}
      ${ANCH_DATE_LIBRARY_DIR}
    )
  endif()

  # build rotate test +
  file(
    GLOB_RECURSE
    test_rotate
    inc/*
    test/testRotate.cpp
  )

  add_executable(
    anch-logger-rotate
    ${test_rotate}
  )

  if(USE_BOOST_RE)
    target_link_libraries(
      anch-logger-rotate
      anch-logger
      anch-resource
      anch-date
      ${CMAKE_THREAD_LIBS_INIT}
      ${Boost_LIBRARIES}
    )
  else()
    target_link_libraries(
      anch-logger-rotate
      anch-logger
      anch-resource
      anch-date
      ${CMAKE_THREAD_LIBS_INIT}
    )
  endif()

  configure_file(test/anch-logger.conf anch-logger.conf COPYONLY)
  # build rotate test -

  # build custom file test +
  file(
    GLOB_RECURSE
    test_custom_file
    inc/*
    test/testCustomFile.cpp
  )

  add_executable(
    anch-logger-custom-file
    ${test_custom_file}
  )

  list(APPEND custom_file_def "_ANCH_LOGGER_CONFIG_FILE_=\"customFile.ini\"")
  set_target_properties(
    anch-logger-custom-file
    PROPERTIES
    COMPILE_DEFINITIONS "${custom_file_def}"
  )

  if(USE_BOOST_RE)
    target_link_libraries(
      anch-logger-custom-file
      anch-logger
      anch-resource
      anch-date
      ${CMAKE_THREAD_LIBS_INIT}
      ${Boost_LIBRARIES}
    )
  else()
    target_link_libraries(
      anch-logger-custom-file
      anch-logger
      anch-resource
      anch-date
      ${CMAKE_THREAD_LIBS_INIT}
    )
  endif()

  configure_file(test/customFile.ini customFile.ini COPYONLY)
  # build custom file test -

  # build low priority file test +
  file(
    GLOB_RECURSE
    test_low_priority_file
    inc/*
    test/testLowPriority.cpp
  )

  add_executable(
    anch-logger-low-priority
    ${test_low_priority_file}
  )

  list(APPEND low_priority_file_def "_ANCH_LOGGER_CONFIG_FILE_=\"low-priority-logger.conf\"")
  set_target_properties(
    anch-logger-low-priority
    PROPERTIES
    COMPILE_DEFINITIONS "${low_priority_file_def}"
  )

  if(USE_BOOST_RE)
    target_link_libraries(
      anch-logger-low-priority
      anch-logger
      anch-resource
      anch-date
      ${CMAKE_THREAD_LIBS_INIT}
      ${Boost_LIBRARIES}
    )
  else()
    target_link_libraries(
      anch-logger-low-priority
      anch-logger
      anch-resource
      anch-date
      ${CMAKE_THREAD_LIBS_INIT}
    )
  endif()

  configure_file(test/low-priority-logger.conf low-priority-logger.conf COPYONLY)
  # build low priority file test -

  # build default configuration test +
  file(
    GLOB_RECURSE
    test_default_conf
    inc/*
    test/testDefaultConfig.cpp
  )

  add_executable(
    anch-logger-default-config
    ${test_default_conf}
  )

  list(APPEND default_conf_def "_ANCH_LOGGER_CONFIG_FILE_=\"invalid.conf\"")
  set_target_properties(
    anch-logger-default-config
    PROPERTIES
    COMPILE_DEFINITIONS "${default_conf_def}"
  )

  if(USE_BOOST_RE)
    target_link_libraries(
      anch-logger-default-config
      anch-logger
      anch-resource
      anch-date
      ${CMAKE_THREAD_LIBS_INIT}
      ${Boost_LIBRARIES}
    )
  else()
    target_link_libraries(
      anch-logger-default-config
      anch-logger
      anch-resource
      anch-date
      ${CMAKE_THREAD_LIBS_INIT}
    )
  endif()
  # build default configuration test -

  enable_testing()
  add_test(anch-logger-rotate ${EXECUTABLE_OUTPUT_PATH}/anch-logger-rotate main)
  add_test(anch-logger-custom-file ${EXECUTABLE_OUTPUT_PATH}/anch-logger-custom-file main)
  add_test(anch-logger-low-priority ${EXECUTABLE_OUTPUT_PATH}/anch-logger-low-priority main)
  add_test(anch-logger-default-config ${EXECUTABLE_OUTPUT_PATH}/anch-logger-default-config main)

endif(ANCH_LOGGER_UNITARY_TEST)
