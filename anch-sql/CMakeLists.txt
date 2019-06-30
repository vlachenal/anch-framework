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
elseif(GCC_VERSION VERSION_GREATER 7.0 OR GCC_VERSION VERSION_EQUAL 7.0)
  message(STATUS "Version >= 7.0 use -std=c++17")
  add_definitions("-std=c++17")
  add_definitions("-D ANCH_STD_OTP")
  add_definitions("-D ANCH_TM_SPEC")
elseif(GCC_VERSION VERSION_GREATER 6.0 OR GCC_VERSION VERSION_EQUAL 6.0)
  message(STATUS "Version >= 6.0 use -std=c++14")
  add_definitions("-std=c++14")
elseif(GCC_VERSION VERSION_GREATER 4.9 OR GCC_VERSION VERSION_EQUAL 4.9)
  message(STATUS "Version >= 4.7 use -std=c++11")
  add_definitions("-std=c++11")
  set(USE_BOOST_RE false)
elseif(GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7)
  message(STATUS "Version >= 4.7 use -std=c++11")
  add_definitions("-std=c++11")
  set(USE_BOOST_RE true)
else()
  message(STATUS "4.5 <= Version < 4.7 use -std=c++0x")
  add_definitions("-std=c++0x")
  set(USE_BOOST_RE true)
endif()
# Check GCC version -

# Add GCC definitions
add_definitions("-Wall -Wextra -Werror -Wuninitialized -Wconversion -Wsign-conversion -Wformat-security -fstack-protector-all -Wstack-protector --param ssp-buffer-size=4 -ftrapv  -z relro,now -fPIC -D_FORTIFY_SOURCE=2")
#add_definitions("-Wall -Wextra -Werror -Wuninitialized -Wconversion -Wsign-conversion -Wformat-security -fstack-protector-all -Wstack-protector --param ssp-buffer-size=4 -ftrapv  -z relro,now -fPIC")
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  add_definitions("-D ANCH_WINDOWS")
else()
  add_definitions("-D ANCH_POSIX")
endif()
add_definitions(${ANCH_SQL_COMPILE_FLAGS})

# Includes location
include_directories(
  inc
  ${ANCH_UTILS_INCLUDES_DIR}
  ${ANCH_RES_INCLUDES_DIR}
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
    inc
    ${MySQL_INCLUDE_DIRS}
    ${ANCH_UTILS_INCLUDES_DIR}
    ${ANCH_RES_INCLUDES_DIR}
    ${ANCH_DATE_INCLUDES_DIR}
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
    inc
    ${PostgreSQL_INCLUDE_DIRS}
    ${ANCH_UTILS_INCLUDES_DIR}
    ${ANCH_RES_INCLUDES_DIR}
    ${ANCH_DATE_INCLUDES_DIR}
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
    ${ANCH_UTILS_INCLUDES_DIR}
    ${ANCH_RES_INCLUDES_DIR}
    ${ANCH_DATE_INCLUDES_DIR}
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

  if(USE_BOOST_RE)
    find_package(Boost COMPONENTS regex)
    link_directories(
      ${Boost_LIBRARY_DIRS}
      ${ANCH_RES_LIBRARY_DIR}
      ${ANCH_DATE_LIBRARY_DIR}
      )
  else()
    link_directories(
      ${ANCH_RES_LIBRARY_DIR}
      ${ANCH_DATE_LIBRARY_DIR}
      )
  endif()

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

    if(USE_BOOST_RE)
      target_link_libraries(
	anch-mysql-test
	anch-sql
	anch-mysql
	anch-resource
	anch-date
	${Boost_LIBRARIES}
	${MySQL_LIBRARIES}
	)
    else()
      target_link_libraries(
	anch-mysql-test
	anch-sql
	anch-mysql
	anch-resource
	anch-date
	${MySQL_LIBRARIES}
	)
    endif()
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

    if(USE_BOOST_RE)
      target_link_libraries(
	anch-postgresql-test
	anch-sql
	anch-postgresql
	anch-resource
	anch-date
	${Boost_LIBRARIES}
	${PostgreSQL_LIBRARIES}
	)
    else()
      target_link_libraries(
	anch-postgresql-test
	anch-sql
	anch-postgresql
	anch-resource
	anch-date
	${PostgreSQL_LIBRARIES}
	)
    endif()
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

    if(USE_BOOST_RE)
      target_link_libraries(
	anch-sqlite3-test
	anch-sql
	anch-sqlite
	anch-resource
	anch-date
	${Boost_LIBRARIES}
	${SQLite3_LIBRARIES}
	)
    else()
      target_link_libraries(
	anch-sqlite3-test
	anch-sql
	anch-sqlite
	anch-resource
	anch-date
	${SQLite3_LIBRARIES}
	)
    endif()
  endif(ANCH_SQL_SQLITE3)
  # Build SQLite3 unitary test -

  # Build SQL connection factory test +
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

  if(USE_BOOST_RE)
    target_link_libraries(
      anch-sql-connection-factory-test
      anch-sql
      anch-resource
      anch-date
      ${Boost_LIBRARIES}
      )
  else()
    target_link_libraries(
      anch-sql-connection-factory-test
      anch-sql
      anch-resource
      anch-date
      )
  endif()
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

  if(USE_BOOST_RE)
    target_link_libraries(
      anch-placeholders-test
      anch-sql
      anch-resource
      anch-date
      ${Boost_LIBRARIES}
      )
  else()
    target_link_libraries(
      anch-placeholders-test
      anch-sql
      anch-resource
      anch-date
      )
  endif()
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

  if(USE_BOOST_RE)
    target_link_libraries(
      anch-select-builder-test
      anch-sql
      anch-resource
      anch-date
      ${Boost_LIBRARIES}
      )
  else()
    target_link_libraries(
      anch-select-builder-test
      anch-sql
      anch-resource
      anch-date
      )
  endif()
  # Build SELECT builder unit tests -

  configure_file(test/anch.db anch.db COPYONLY)
  configure_file(test/db_con.conf db_con.conf COPYONLY)
  enable_testing()
  if(ANCH_SQL_MYSQL)
    add_test(MySQL ${EXECUTABLE_OUTPUT_PATH}/anch-mysql-test main)
    add_test(SQLConnectionFactoryMySQL ${EXECUTABLE_OUTPUT_PATH}anch-sql-connection-factory-test MySQL)
  endif(ANCH_SQL_MYSQL)
  if(ANCH_SQL_POSTGRESQL)
    add_test(PostgreSQL ${EXECUTABLE_OUTPUT_PATH}/anch-postgresql-test main)
    add_test(SQLConnectionFactoryPostgreSQL ${EXECUTABLE_OUTPUT_PATH}anch-sql-connection-factory-test PostgreSQL)
  endif(ANCH_SQL_POSTGRESQL)
  if(ANCH_SQL_SQLITE3)
    add_test(SQLite3 ${EXECUTABLE_OUTPUT_PATH}/anch-sqlite3-test main)
    add_test(SQLConnectionFactorySQLite ${EXECUTABLE_OUTPUT_PATH}anch-sql-connection-factory-test SQLite)
  endif(ANCH_SQL_SQLITE3)
  add_test(SQLPrepareStatementPlaceholders ${EXECUTABLE_OUTPUT_PATH}anch-placeholders-test main)
  # SELECT builder unit test +
  add_test(SELECTwithoutFROMnorWHERE ${EXECUTABLE_OUTPUT_PATH}anch-select-builder-test 0_from_no_where)
  add_test(SELECT1columnFROMtableWhitoutWHERE ${EXECUTABLE_OUTPUT_PATH}anch-select-builder-test 1_from_no_where)
  add_test(SELECTDISTINCT1columnFROMtableWhitoutWHERE ${EXECUTABLE_OUTPUT_PATH}anch-select-builder-test distinct_1_from_no_where)
  add_test(SELECT1columnWithAliasFROMtableWhitoutWHERE ${EXECUTABLE_OUTPUT_PATH}anch-select-builder-test 1_as_from_no_where)
  add_test(SELECT2columnsFROMtableWhitoutWHERE ${EXECUTABLE_OUTPUT_PATH}anch-select-builder-test 2_from_no_where)
  add_test(SelfJoin ${EXECUTABLE_OUTPUT_PATH}anch-select-builder-test self_join_no_where)
  add_test(SelfJoinSubquery ${EXECUTABLE_OUTPUT_PATH}anch-select-builder-test self_join_subquery_no_where)
  add_test(NaturalJoin ${EXECUTABLE_OUTPUT_PATH}anch-select-builder-test natural_join_no_where)
  add_test(NaturalJoinSubquery ${EXECUTABLE_OUTPUT_PATH}anch-select-builder-test natural_join_subquery_no_where)
  add_test(CrossJoin ${EXECUTABLE_OUTPUT_PATH}anch-select-builder-test cross_join_no_where)
  add_test(CrossJoinSubquery ${EXECUTABLE_OUTPUT_PATH}anch-select-builder-test cross_join_subquery_no_where)
  # SELECT builder unit test -

endif(ANCH_SQL_UNITARY_TEST)
