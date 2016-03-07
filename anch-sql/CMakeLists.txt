cmake_minimum_required(VERSION 2.6)

# Project anch SQL
# ANother C++ Harsh/Huge/Hack
project(anch-sql)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include(AnchSQL.cmake.in)

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
add_definitions("-Wall -Wextra -Werror -Wuninitialized")
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  add_definitions("-D ANCH_WINDOWS")
else()
  add_definitions("-D ANCH_POSIX")
endif()
add_definitions(${ANCH_SQL_COMPILE_FLAGS})

# Include librairies
set(CMAKE_MODULE_PATH "$ENV{HOME}/projects/anch-framework/anch-sql/;${CMAKE_MODULE_PATH}")
# MySQL +
if(ANCH_SQL_MYSQL)
  find_package(MySQL)
  if(MySQL_FOUND)
    add_definitions("-D ANCH_SQL_MYSQL")
    message(STATUS "Found MySQL version: ${MySQL_VERSION_STRING}")
    message(STATUS "Found MySQL include directory: ${MySQL_INCLUDE_DIRS}")
    message(STATUS "Found MySQL libraries: ${MySQL_LIBRARIES}")
  else()
    message(FATAL_ERROR "MySQL libraries not found")
  endif(MySQL_FOUND)
endif(ANCH_SQL_MYSQL)
# MySQL -

# PostgreSQL +
if(ANCH_SQL_POSTGRESQL)
  find_package(PostgreSQL)
  if(PostgreSQL_FOUND)
    add_definitions("-D ANCH_SQL_POSTGRESQL")
    message(STATUS "Found PostgreSQL version: ${PostgreSQL_VERSION_STRING}")
    message(STATUS "Found PostgreSQL include directory: ${PostgreSQL_INCLUDE_DIRS}")
    message(STATUS "Found PostgreSQL libraries: ${PostgreSQL_LIBRARIES}")
  else()
    message(FATAL_ERROR "PostgreSQL libraries not found")
  endif(PostgreSQL_FOUND)
endif(ANCH_SQL_POSTGRESQL)
# PostgreSQL -

# SQLite3 +
if(ANCH_SQL_SQLITE3)
  find_package(SQLite3)
  if(SQLite3_FOUND)
    add_definitions("-D ANCH_SQL_SQLITE3")
    message(STATUS "Found SQLite3 version: ${SQLite3_VERSION_STRING}")
    message(STATUS "Found SQLite3 include directory: ${SQLite3_INCLUDE_DIRS}")
    message(STATUS "Found SQLite3 libraries: ${SQLite3_LIBRARIES}")
  else()
    message(FATAL_ERROR "SQLite3 libraries not found")
  endif(SQLite3_FOUND)
endif(ANCH_SQL_SQLITE3)
# SQLite3 -

# Includes location
include_directories(
  inc
  ${MySQL_INCLUDE_DIRS}
  ${PostgreSQL_INCLUDE_DIRS}
  ${SQLite3_INCLUDE_DIRS}
  ${ANCH_UTILS_INCLUDES_DIR}
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
  anch-sql
  SHARED
  ${source_files}
)

# Unitary tests
if(ANCH_SQL_UNITARY_TEST)

  # Build MySQL unitary test +
  if(ANCH_SQL_MYSQL)
    file(
      GLOB_RECURSE
      test_mysql
      inc/*
      test/testMySQL.cpp
      )

    add_executable(
      anch-mysql-test
      ${test_mysql}
      )

    target_link_libraries(
      anch-mysql-test
      anch-sql
      ${MySQL_LIBRARIES}
      ${PostgreSQL_LIBRARIES}
      )
  endif(ANCH_SQL_MYSQL)
  # Build MySQL unitary test -

  # Build PostgreSQL unitary test +
  if(ANCH_SQL_POSTGRESQL)
    file(
      GLOB_RECURSE
      test_postgresql
      inc/*
      test/testPostgreSQL.cpp
      )

    add_executable(
      anch-postgresql-test
      ${test_postgresql}
      )

    target_link_libraries(
      anch-postgresql-test
      anch-sql
      ${MySQL_LIBRARIES}
      ${PostgreSQL_LIBRARIES}
      )
  endif(ANCH_SQL_POSTGRESQL)
  # Build PostgreSQL unitary test -

  enable_testing()
  if(ANCH_SQL_MYSQL)
    add_test(MySQL ${EXECUTABLE_OUTPUT_PATH}/anch-mysql-test main)
  endif(ANCH_SQL_MYSQL)
  if(ANCH_SQL_POSTGRESQL)
    add_test(PostgreSQL ${EXECUTABLE_OUTPUT_PATH}/anch-postgresql-test main)
  endif(ANCH_SQL_POSTGRESQL)

endif(ANCH_SQL_UNITARY_TEST)
