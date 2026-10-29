# AnCH SQL: SQL connections, connection pools and query builder
anch_add_library(anch-sql
  PUBLIC_DEPS anch::conf anch::date anch::utils
  PRIVATE_DEPS ${CMAKE_DL_LIBS})

# Database drivers +
# Drivers are loaded at runtime by anch-sql (dlopen of libanch-<driver>.so)
# but can also be linked directly.
#
# anch_add_sql_driver(<driver> <dir> <definition> <database library target>)
function(anch_add_sql_driver driver dir definition db_target)
  file(GLOB_RECURSE sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/src/*.cpp)
  anch_add_library(anch-${driver}
    INCLUDE_DIRS ${dir}/inc
    SOURCES ${sources}
    PUBLIC_DEPS anch::sql ${db_target}
    PUBLIC_DEFINITIONS ${definition})
endfunction()

if(ANCH_SQL_MYSQL)
  find_package(PkgConfig REQUIRED)
  pkg_search_module(MySQL REQUIRED IMPORTED_TARGET libmariadb mysqlclient)
  anch_add_sql_driver(mysql mysql ANCH_SQL_MYSQL PkgConfig::MySQL)
endif()

if(ANCH_SQL_POSTGRESQL)
  find_package(PostgreSQL REQUIRED)
  anch_add_sql_driver(postgresql postgresql ANCH_SQL_POSTGRESQL PostgreSQL::PostgreSQL)
endif()

if(ANCH_SQL_SQLITE3)
  find_package(SQLite3 REQUIRED)
  # SQLite::SQLite3 has been renamed SQLite3::SQLite3 in recent CMake versions
  if(TARGET SQLite3::SQLite3)
    set(sqlite_target SQLite3::SQLite3)
  else()
    set(sqlite_target SQLite::SQLite3)
  endif()
  anch_add_sql_driver(sqlite sqlite ANCH_SQL_SQLITE3 ${sqlite_target})
endif()
# Database drivers -

if(ANCH_BUILD_TESTS)
  add_subdirectory(test)
endif()
