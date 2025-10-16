cmake_minimum_required(VERSION 3.10)

# Project anch SQL
# ANother C++ Harsh/Huge/Hack
project(anch-sql)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include(AnchSQL.cmake.in)

# Check GCC version +
execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
  OUTPUT_VARIABLE GCC_VERSION)
if (GCC_VERSION VERSION_LESS 13.0)
  message(FATAL_ERROR "Version < 13.0. This library is based on C++23 standard. Exit.")
else()
  message(STATUS "Version >= 13.0 use -std=c++23")
  add_definitions("-std=c++23")
endif()
# Check GCC version -

# Add GCC definitions
add_definitions("-Wall -Wextra -Werror -Wuninitialized -Wconversion -Wsign-conversion -Wformat-security -fstack-protector-all -Wstack-protector --param ssp-buffer-size=4 -ftrapv -D_FORTIFY_SOURCE=2 -z relro,now -fPIC")
#add_definitions("-Wall -Wextra -Werror -Wuninitialized -Wconversion -Wsign-conversion -Wformat-security -fstack-protector-all -Wstack-protector --param ssp-buffer-size=4 -ftrapv -z relro,now -fPIC")
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  add_definitions("-D ANCH_WINDOWS")
else()
  add_definitions("-D ANCH_POSIX")
endif()
add_definitions(" ${ANCH_SQL_COMPILE_FLAGS}")

# Includes location
include_directories(
  inc
  ${ANCH_UTILS_INCLUDES_DIR}
  ${ANCH_CONF_INCLUDES_DIR}
  ${ANCH_INI_INCLUDES_DIR}
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
  anch-sql-obj
  OBJECT
  ${source_files}
)

## Dynamic library +
add_library(
  anch-sql
  SHARED
  $<TARGET_OBJECTS:anch-sql-obj>
)

target_link_libraries(
  anch-sql
  ${CMAKE_DL_LIBS}
)

install(
  DIRECTORY inc/ DESTINATION ${ANCH_SQL_INSTALL_INC_DIR}
  FILES_MATCHING PATTERN "*.hpp"
)

install(
  TARGETS anch-sql
  LIBRARY DESTINATION ${ANCH_SQL_INSTALL_LIB_DIR}
)
## Dynamic library -

## Static library +
if(ANCH_SQL_STATIC_LIB)
  add_library(
    anch-sql-static
    STATIC
    $<TARGET_OBJECTS:anch-sql-obj>
  )
  set_target_properties(anch-sql-static PROPERTIES OUTPUT_NAME anch-sql)

  install(
    TARGETS anch-sql-static
    ARCHIVE DESTINATION ${ANCH_SQL_INSTALL_LIB_DIR}
  )
endif(ANCH_SQL_STATIC_LIB)
## Static library -

## Shared libraries +
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

  # MySQL includes location
  include_directories(
    mysql/inc
    ${MySQL_INCLUDE_DIRS}
  )

  # Retrieve every files in MySQL project
  file(
    GLOB_RECURSE
    mysql_source_files
    mysql/*
  )

  # Build objects
  add_library(
    anch-mysql-obj
    OBJECT
    ${mysql_source_files}
  )

  ## Dynamic library +
  add_library(
    anch-mysql
    SHARED
    $<TARGET_OBJECTS:anch-mysql-obj>
  )
  target_link_libraries(anch-mysql ${MySQL_LIBRARIES})

  install(
    DIRECTORY mysql/inc/ DESTINATION ${ANCH_SQL_INSTALL_INC_DIR}
    FILES_MATCHING PATTERN "*.hpp"
  )

  install(
    TARGETS anch-mysql
    LIBRARY DESTINATION ${ANCH_SQL_INSTALL_LIB_DIR}
  )
  ## Dynamic library -

  ## Static library +
  if(ANCH_SQL_STATIC_LIB)
    add_library(
      anch-mysql-static
      STATIC
      $<TARGET_OBJECTS:anch-mysql-obj>
    )
    set_target_properties(anch-mysql-static PROPERTIES OUTPUT_NAME anch-mysql)
    target_link_libraries(anch-mysql-static ${MySQL_LIBRARIES})

    install(
      TARGETS anch-mysql-static
      ARCHIVE DESTINATION ${ANCH_SQL_INSTALL_LIB_DIR}
    )
  endif(ANCH_SQL_STATIC_LIB)
  ## Static library -

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

  # PostgreSQL includes location
  include_directories(
    postgresql/inc
    ${PostgreSQL_INCLUDE_DIRS}
  )

  # Retrieve every files in PostgreSQL project
  file(
    GLOB_RECURSE
    pg_source_files
    postgresql/*
  )

  # Build objects
  add_library(
    anch-postgresql-obj
    OBJECT
    ${pg_source_files}
  )

  ## Dynamic library +
  add_library(
    anch-postgresql
    SHARED
    $<TARGET_OBJECTS:anch-postgresql-obj>
  )
  target_link_libraries(anch-postgresql ${PostgreSQL_LIBRARIES})

  install(
    DIRECTORY postgresql/inc/ DESTINATION ${ANCH_SQL_INSTALL_INC_DIR}
    FILES_MATCHING PATTERN "*.hpp"
  )

  install(
    TARGETS anch-postgresql
    LIBRARY DESTINATION ${ANCH_SQL_INSTALL_LIB_DIR}
  )
  ## Dynamic library -

  ## Static library +
  if(ANCH_SQL_STATIC_LIB)
    add_library(
      anch-postgresql-static
      STATIC
      $<TARGET_OBJECTS:anch-postgresql-obj>
    )
    set_target_properties(anch-postgresql-static PROPERTIES OUTPUT_NAME anch-postgresql)
    target_link_libraries(anch-postgresql-static ${PostgreSQL_LIBRARIES})

    install(
      TARGETS anch-postgresql-static
      ARCHIVE DESTINATION ${ANCH_SQL_INSTALL_LIB_DIR}
    )
  endif(ANCH_SQL_STATIC_LIB)
  ## Static library -

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

  # SQLite3 includes location
  include_directories(
    sqlite/inc
    inc
    ${SQLite3_INCLUDE_DIRS}
  )

  # Retrieve every files in PostgreSQL project
  file(
    GLOB_RECURSE
    sqlite_source_files
    sqlite/*
  )

  # Build objects
  add_library(
    anch-sqlite-obj
    OBJECT
    ${sqlite_source_files}
  )

  ## Dynamic library +
  add_library(
    anch-sqlite
    SHARED
    $<TARGET_OBJECTS:anch-sqlite-obj>
  )
  target_link_libraries(anch-sqlite ${SQLite3_LIBRARIES})

  install(
    DIRECTORY sqlite/inc/ DESTINATION ${ANCH_SQL_INSTALL_INC_DIR}
    FILES_MATCHING PATTERN "*.hpp"
  )

  install(
    TARGETS anch-sqlite
    LIBRARY DESTINATION ${ANCH_SQL_INSTALL_LIB_DIR}
  )
  ## Dynamic library -

  ## Static library +
  if(ANCH_SQL_STATIC_LIB)
    add_library(
      anch-sqlite-static
      STATIC
      $<TARGET_OBJECTS:anch-sqlite-obj>
    )
    set_target_properties(anch-sqlite-static PROPERTIES OUTPUT_NAME anch-sqlite)
    target_link_libraries(anch-sqlite-static ${SQLite3_LIBRARIES})

    install(
      TARGETS anch-sqlite-static
      ARCHIVE DESTINATION ${ANCH_SQL_INSTALL_LIB_DIR}
    )
  endif(ANCH_SQL_STATIC_LIB)
  ## Static library -

endif(ANCH_SQL_SQLITE3)
# SQLite3 -
## Shared libraries -

# Unitary tests
if(ANCH_SQL_UNITARY_TEST)

  include_directories(
    ${ANCH_UT_INCLUDES_DIR}
    ${ANCH_CLI_INCLUDES_DIR}
  )

  link_directories(
    ${ANCH_CONF_LIBRARY_DIR}
    ${ANCH_INI_LIBRARY_DIR}
    ${ANCH_DATE_LIBRARY_DIR}
    ${ANCH_UT_LIBRARY_DIR}
    ${ANCH_CLI_LIBRARY_DIR}
  )

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
      anch-mysql
      anch-conf
      anch-ini
      anch-date
      anch-unit
      anch-cli
      ${MySQL_LIBRARIES}
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
      anch-postgresql
      anch-conf
      anch-ini
      anch-date
      anch-unit
      anch-cli
      ${PostgreSQL_LIBRARIES}
    )
  endif(ANCH_SQL_POSTGRESQL)
  # Build PostgreSQL unitary test -

  # Build SQLite3 unitary test +
  if(ANCH_SQL_SQLITE3)
    file(
      GLOB_RECURSE
      test_sqlite3
      inc/*
      test/testSQLite3.cpp
    )

    add_executable(
      anch-sqlite3-test
      ${test_sqlite3}
    )

    target_link_libraries(
      anch-sqlite3-test
      anch-sql
      anch-sqlite
      anch-conf
      anch-ini
      anch-date
      anch-unit
      anch-cli
      ${SQLite3_LIBRARIES}
    )
  endif(ANCH_SQL_SQLITE3)
  # Build SQLite3 unitary test -

  # Build SQL connection factory test +
  set(ENV{LD_LIBRARY_PATH} ${LIBRARY_OUTPUT_DIRECTORY})
  file(
    GLOB_RECURSE
    test_sqlfactory
    inc/*
    test/testSQLConnectionFactory.cpp
  )

  add_executable(
    anch-sql-connection-factory-test
    ${test_sqlfactory}
  )

  target_link_libraries(
    anch-sql-connection-factory-test
    anch-sql
    anch-conf
    anch-ini
    anch-date
    anch-unit
    anch-cli
  )
  # Build SQL connection factory test -

  # Build SQL prepare statement placeholders +
  file(
    GLOB_RECURSE
    test_placeholders
    inc/*
    test/testPrepareStatementWildCards.cpp
  )

  add_executable(
    anch-placeholders-test
    ${test_placeholders}
  )

  target_link_libraries(
    anch-placeholders-test
    anch-sql
    anch-conf
    anch-ini
    anch-date
    anch-unit
    anch-cli
  )
  # Build SQL prepare statement placeholders -

  # Build SELECT builder unit tests +
  file(
    GLOB_RECURSE
    test_select_builder
    inc/*
    test/testSelectBuilder.cpp
  )

  add_executable(
    anch-select-builder-test
    ${test_select_builder}
  )

  target_link_libraries(
    anch-select-builder-test
    anch-sql
    anch-conf
    anch-ini
    anch-date
    anch-unit
    anch-cli
  )
  # Build SELECT builder unit tests -

  configure_file(test/anch.db anch.db COPYONLY)
  configure_file(test/db_con.conf db_con.conf COPYONLY)
  enable_testing()
  if(ANCH_SQL_MYSQL)
    add_test(MySQL-query-manual-handling ${EXECUTABLE_OUTPUT_PATH}/anch-mysql-test -t 01-query-manual-handling)
    add_test(MySQL-query-row-result-handling ${EXECUTABLE_OUTPUT_PATH}/anch-mysql-test -t 02-query-row-result-handling)
    add_test(MySQL-query-result-set-handling ${EXECUTABLE_OUTPUT_PATH}/anch-mysql-test -t 03-query-result-set-handling)
    add_test(MySQL-insert ${EXECUTABLE_OUTPUT_PATH}/anch-mysql-test -t 04-insert)
    add_test(MySQL-update ${EXECUTABLE_OUTPUT_PATH}/anch-mysql-test -t 05-update)
    add_test(MySQL-query-stmt-row-result-handling ${EXECUTABLE_OUTPUT_PATH}/anch-mysql-test -t 07-query-stmt-row-result-handling)
    add_test(MySQL-insert-stmt ${EXECUTABLE_OUTPUT_PATH}/anch-mysql-test -t 08-insert-stmt)
    add_test(MySQL-update-stmt ${EXECUTABLE_OUTPUT_PATH}/anch-mysql-test -t 09-update-stmt)
    add_test(MySQL-transactions ${EXECUTABLE_OUTPUT_PATH}/anch-mysql-test -t 10-transactions)

    add_test(SQLConnectionFactoryMySQL ${EXECUTABLE_OUTPUT_PATH}anch-sql-connection-factory-test -t sql-factory-mysql)
    set_tests_properties(SQLConnectionFactoryMySQL PROPERTIES ENVIRONMENT "LD_LIBRARY_PATH=./lib")
  endif(ANCH_SQL_MYSQL)

  if(ANCH_SQL_POSTGRESQL)
    add_test(PostgreSQL-query-manual-handling ${EXECUTABLE_OUTPUT_PATH}/anch-postgresql-test -t 01-query-manual-handling)
    add_test(PostgreSQL-query-row-result-handling ${EXECUTABLE_OUTPUT_PATH}/anch-postgresql-test -t 02-query-row-result-handling)
    add_test(PostgreSQL-query-result-set-handling ${EXECUTABLE_OUTPUT_PATH}/anch-postgresql-test -t 03-query-result-set-handling)
    add_test(PostgreSQL-insert ${EXECUTABLE_OUTPUT_PATH}/anch-postgresql-test -t 04-insert)
    add_test(PostgreSQL-update ${EXECUTABLE_OUTPUT_PATH}/anch-postgresql-test -t 05-update)
    add_test(PostgreSQL-query-stmt-row-result-handling ${EXECUTABLE_OUTPUT_PATH}/anch-postgresql-test -t 07-query-stmt-row-result-handling)
    add_test(PostgreSQL-insert-stmt ${EXECUTABLE_OUTPUT_PATH}/anch-postgresql-test -t 08-insert-stmt)
    add_test(PostgreSQL-update-stmt ${EXECUTABLE_OUTPUT_PATH}/anch-postgresql-test -t 09-update-stmt)
    add_test(PostgreSQL-transactions ${EXECUTABLE_OUTPUT_PATH}/anch-postgresql-test -t 10-transactions)

    add_test(SQLConnectionFactoryPostgreSQL ${EXECUTABLE_OUTPUT_PATH}anch-sql-connection-factory-test -t sql-factory-postgresql)
    set_tests_properties(SQLConnectionFactoryPostgreSQL PROPERTIES ENVIRONMENT "LD_LIBRARY_PATH=./lib")
  endif(ANCH_SQL_POSTGRESQL)

  if(ANCH_SQL_SQLITE3)
    add_test(SQLite3-query-manual-handling ${EXECUTABLE_OUTPUT_PATH}/anch-sqlite3-test -t query-manual-handling)
    add_test(SQLite3-query-row-result-handling ${EXECUTABLE_OUTPUT_PATH}/anch-sqlite3-test -t query-row-result-handling)
    add_test(SQLite3-query-result-set-handling ${EXECUTABLE_OUTPUT_PATH}/anch-sqlite3-test -t query-result-set-handling)
    add_test(SQLite3-insert ${EXECUTABLE_OUTPUT_PATH}/anch-sqlite3-test -t insert)
    add_test(SQLite3-update ${EXECUTABLE_OUTPUT_PATH}/anch-sqlite3-test -t update)
    add_test(SQLite3-query-stmt-row-result-handling ${EXECUTABLE_OUTPUT_PATH}/anch-sqlite3-test -t query-stmt-row-result-handling)
    add_test(SQLite3-insert-stmt ${EXECUTABLE_OUTPUT_PATH}/anch-sqlite3-test -t insert-stmt)
    add_test(SQLite3-update-stmt ${EXECUTABLE_OUTPUT_PATH}/anch-sqlite3-test -t update-stmt)
    add_test(SQLite3-transactions ${EXECUTABLE_OUTPUT_PATH}/anch-sqlite3-test -t transactions)

    add_test(SQLConnectionFactorySQLite ${EXECUTABLE_OUTPUT_PATH}anch-sql-connection-factory-test -t sql-factory-sqlite3)
    set_tests_properties(SQLConnectionFactorySQLite PROPERTIES ENVIRONMENT "LD_LIBRARY_PATH=./lib")
  endif(ANCH_SQL_SQLITE3)

  # Prepared statement placeholders +
  add_test(SqlStmtWC-select-2-wildcards ${EXECUTABLE_OUTPUT_PATH}anch-placeholders-test -t select-2-wildcards)
  add_test(SqlStmtWC-select-2-wildcards-1-fixed ${EXECUTABLE_OUTPUT_PATH}anch-placeholders-test -t select-2-wildcards-1-fixed)
  add_test(SqlStmtWC-select-1-wildcard-1-fixed-1-wildcard ${EXECUTABLE_OUTPUT_PATH}anch-placeholders-test -t select-1-wildcard-1-fixed-1-wildcard)
  add_test(SqlStmtWC-select-1-wildcard-1-fixed-quote-1-wildcard ${EXECUTABLE_OUTPUT_PATH}anch-placeholders-test -t select-1-wildcard-1-fixed-quote-1-wildcard)
  add_test(SqlStmtWC-select-1-wildcard-1-fixed-backslash-1-wildcard ${EXECUTABLE_OUTPUT_PATH}anch-placeholders-test -t select-1-wildcard-1-fixed-backslash-1-wildcard)
  add_test(SqlStmtWC-select-1-wildcard-1-fixed-quoteafter-1-wildcard ${EXECUTABLE_OUTPUT_PATH}anch-placeholders-test -t select-1-wildcard-1-fixed-quoteafter-1-wildcard)
  add_test(SqlStmtWC-select-1-wildcard-1-fixed-quotebefore-1-wildcard ${EXECUTABLE_OUTPUT_PATH}anch-placeholders-test -t select-1-wildcard-1-fixed-quotebefore-1-wildcard)
  add_test(SqlStmtWC-insert-2-wildcards ${EXECUTABLE_OUTPUT_PATH}anch-placeholders-test -t insert-2-wildcards)
  add_test(SqlStmtWC-insert-1-wildcard-1fixed-1-wildcard ${EXECUTABLE_OUTPUT_PATH}anch-placeholders-test -t insert-1-wildcard-1fixed-1-wildcard)
  add_test(SqlStmtWC-insert-1-wildcard-1fixed-quote-1-wildcard ${EXECUTABLE_OUTPUT_PATH}anch-placeholders-test -t insert-1-wildcard-1fixed-quote-1-wildcard)
  add_test(SqlStmtWC-insert-1-wildcard-1fixed-backslash-1-wildcard ${EXECUTABLE_OUTPUT_PATH}anch-placeholders-test -t insert-1-wildcard-1fixed-backslash-1-wildcard)
  add_test(SqlStmtWC-insert-1-wildcard-1fixed-quoteafter-1-wildcard ${EXECUTABLE_OUTPUT_PATH}anch-placeholders-test -t insert-1-wildcard-1fixed-quoteafter-1-wildcard)
  add_test(SqlStmtWC-insert-1-wildcard-1fixed-quotebefore-1-wildcard ${EXECUTABLE_OUTPUT_PATH}anch-placeholders-test -t insert-1-wildcard-1fixed-quotebefore-1-wildcard)
  add_test(SqlStmtWC-insert-1-wildcard-1fixed-robracket-1-wildcard ${EXECUTABLE_OUTPUT_PATH}anch-placeholders-test -t insert-1-wildcard-1fixed-robracket-1-wildcard)
  add_test(SqlStmtWC-insert-1-wildcard-1fixed-rcbracket-1-wildcard ${EXECUTABLE_OUTPUT_PATH}anch-placeholders-test -t insert-1-wildcard-1fixed-rcbracket-1-wildcard)
  add_test(SqlStmtWC-select-in ${EXECUTABLE_OUTPUT_PATH}anch-placeholders-test -t select-in)
  add_test(SqlStmtWC-select-in-1F ${EXECUTABLE_OUTPUT_PATH}anch-placeholders-test -t select-in-1F)
  add_test(SqlStmtWC-select-postgres ${EXECUTABLE_OUTPUT_PATH}anch-placeholders-test -t select-postgres)
  # Prepared statement placeholders -

  # SELECT builder unit test +
  add_test(SELECTwithoutFROMnorWHERE ${EXECUTABLE_OUTPUT_PATH}anch-select-builder-test -t 0_from_no_where)
  add_test(SELECT1columnFROMtableWhitoutWHERE ${EXECUTABLE_OUTPUT_PATH}anch-select-builder-test -t 1_from_no_where)
  add_test(SELECTDISTINCT1columnFROMtableWhitoutWHERE ${EXECUTABLE_OUTPUT_PATH}anch-select-builder-test -t distinct_1_from_no_where)
  add_test(SELECT1columnWithAliasFROMtableWhitoutWHERE ${EXECUTABLE_OUTPUT_PATH}anch-select-builder-test -t 1_as_from_no_where)
  add_test(SELECT2columnsFROMtableWhitoutWHERE ${EXECUTABLE_OUTPUT_PATH}anch-select-builder-test -t 2_from_no_where)
  add_test(SelfJoin ${EXECUTABLE_OUTPUT_PATH}anch-select-builder-test -t self_join_no_where)
  add_test(SelfJoinSubquery ${EXECUTABLE_OUTPUT_PATH}anch-select-builder-test -t self_join_subquery_no_where)
  add_test(NaturalJoin ${EXECUTABLE_OUTPUT_PATH}anch-select-builder-test -t natural_join_no_where)
  add_test(NaturalJoinSubquery ${EXECUTABLE_OUTPUT_PATH}anch-select-builder-test -t natural_join_subquery_no_where)
  add_test(CrossJoin ${EXECUTABLE_OUTPUT_PATH}anch-select-builder-test -t cross_join_no_where)
  add_test(CrossJoinSubquery ${EXECUTABLE_OUTPUT_PATH}anch-select-builder-test -t cross_join_subquery_no_where)
  # SELECT builder unit test -

endif(ANCH_SQL_UNITARY_TEST)
