set_property(DIRECTORY PROPERTY LABELS log)

anch_copy_test_files(
  anch-logger.conf
  customFile.ini
  low-priority-logger.conf
  anch-logger-bad-writer.conf
  anch-logger-bad-logger.conf)

anch_add_executable(anch-logger-test SOURCES testLogger.cpp LIBS anch::log anch::ut)

add_test(NAME anch-logger-rotate COMMAND anch-logger-test -t rotate)
add_test(NAME anch-logger-custom-file COMMAND anch-logger-test -t custom-file)
add_test(NAME anch-logger-low-priority COMMAND anch-logger-test -t low-priority)
add_test(NAME anch-logger-default-config COMMAND anch-logger-test -t default)
add_test(NAME anch-logger-bad-writers COMMAND anch-logger-test -t bad-writers)
add_test(NAME anch-logger-bad-loggers COMMAND anch-logger-test -t bad-loggers)

# Logger tests share the same log files: never run them in parallel
get_property(log_tests DIRECTORY PROPERTY TESTS)
set_tests_properties(${log_tests} PROPERTIES RESOURCE_LOCK anch-logger-files)
