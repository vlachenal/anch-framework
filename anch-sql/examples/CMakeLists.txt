set_property(DIRECTORY PROPERTY LABELS sql)

# Examples need MySQL and PostgreSQL servers: they are built, not run
if(ANCH_SQL_POSTGRESQL)
  anch_add_example(sql-connection SOURCES connection.cpp LIBS anch::postgresql)
endif()
anch_add_example(sql-pool SOURCES pool.cpp LIBS anch::sql)
