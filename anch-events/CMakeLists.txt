cmake_minimum_required(VERSION 2.6)

# Project anch events
# ANother C++ Harsh/Huge/Hack
project(anch-events)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include(AnchEvents.cmake.in)

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
add_definitions("-Wall -Wextra -Werror")

# Include librairies
include(FindThreads)
find_package(Threads)

# Includes location
include_directories(
  inc
)

if(ANCH_EVENTS_UNITARY_TEST)

  # Build observer/observable unitary test +
  file(
    GLOB_RECURSE
    observ_files
    inc/*
    test/testEvents.cpp
  )

  add_executable(
    anch-observ-test
    ${observ_files}
  )

  target_link_libraries(
    anch-observ-test
    ${CMAKE_THREAD_LIBS_INIT}
  )
  # Build observer/observable unitary test -

  # Build event handler unitary test +
  file(
    GLOB_RECURSE
    handler_files
    inc/*
    test/testMultiHandler.cpp
  )

  add_executable(
    anch-handlers-test
    ${handler_files}
  )

  target_link_libraries(
    anch-handlers-test
    ${CMAKE_THREAD_LIBS_INIT}
  )
  # Build event handler unitary test -

  # Build event bus unitary test +
  file(
    GLOB_RECURSE
    bus_files
    inc/*
    test/testEventBus.cpp
  )

  add_executable(
    anch-event-bus-test
    ${bus_files}
  )

  target_link_libraries(
    anch-event-bus-test
    ${CMAKE_THREAD_LIBS_INIT}
  )
  # Build event bus unitary test -

  enable_testing()
  add_test(anch-observ-test ${EXECUTABLE_OUTPUT_PATH}/anch-observ-test main)
  add_test(anch-handlers-test ${EXECUTABLE_OUTPUT_PATH}/anch-handlers-test main)
  add_test(anch-event-bus-test ${EXECUTABLE_OUTPUT_PATH}/anch-event-bus-test main)

endif(ANCH_EVENTS_UNITARY_TEST)
