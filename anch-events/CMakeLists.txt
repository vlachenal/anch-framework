cmake_minimum_required(VERSION 3.10)

# Project anch events
# ANother C++ Harsh/Huge/Hack
project(anch-events)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include(AnchEvents.cmake.in)

# Check GCC version +
execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
  OUTPUT_VARIABLE GCC_VERSION)
if (GCC_VERSION VERSION_LESS 8.0)
  message(FATAL_ERROR "Version < 8.0. This library is based on C++17 standard. Exit.")
elseif(GCC_VERSION VERSION_GREATER 10.0 OR GCC_VERSION VERSION_EQUAL 10.0)
  message(STATUS "Version >= 10.0 use -std=c++20")
  add_definitions("-std=c++20")
else()
  message(STATUS "Version >= 8.0 use -std=c++17")
  add_definitions("-std=c++17")
endif()
# Check GCC version -

# Add GCC definitions
add_definitions("-Wall -Wextra -Werror -Wuninitialized -Wconversion -Wsign-conversion -Wformat-security -fstack-protector-all -Wstack-protector --param ssp-buffer-size=4 -ftrapv  -z relro,now -fPIC -D_FORTIFY_SOURCE=2")
add_definitions(${ANCH_EVENTS_COMPILE_FLAGS})

# Includes location
include_directories(
  inc
  ${ANCH_UTILS_INCLUDES_DIR}
)

install(
  DIRECTORY inc/ DESTINATION ${ANCH_EVENTS_INSTALL_INC_DIR}
  FILES_MATCHING PATTERN "*.hpp"
)

if(ANCH_EVENTS_UNITARY_TEST)

  # Build observer/observable unitary test +
  file(
    GLOB_RECURSE
    observ_files
    test/testEvents.cpp
  )

  add_executable(
    anch-observ-test
    ${observ_files}
  )

  target_link_libraries(
    anch-observ-test
  )
  # Build observer/observable unitary test -

  # Build event handler unitary test +
  file(
    GLOB_RECURSE
    handler_files
    test/testMultiHandler.cpp
  )

  add_executable(
    anch-handlers-test
    ${handler_files}
  )

  target_link_libraries(
    anch-handlers-test
  )
  # Build event handler unitary test -

  # Build event bus unitary test +
  file(
    GLOB_RECURSE
    bus_files
    test/testEventBus.cpp
  )

  add_executable(
    anch-event-bus-test
    ${bus_files}
  )

  target_link_libraries(
    anch-event-bus-test
  )
  # Build event bus unitary test -

  enable_testing()
  add_test(anch-observ-test ${EXECUTABLE_OUTPUT_PATH}/anch-observ-test main)
  add_test(anch-handlers-test ${EXECUTABLE_OUTPUT_PATH}/anch-handlers-test main)
  add_test(anch-event-bus-test ${EXECUTABLE_OUTPUT_PATH}/anch-event-bus-test main)

endif(ANCH_EVENTS_UNITARY_TEST)
