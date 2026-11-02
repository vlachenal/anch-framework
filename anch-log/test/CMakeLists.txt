set_property(DIRECTORY PROPERTY LABELS log)

anch_copy_test_files(
  anch-logger.conf
  customFile.ini
  low-priority-logger.conf
  anch-logger-bad-writer.conf
  anch-logger-bad-logger.conf
  anch-logger-writers.conf)

anch_add_executable(anch-logger-test SOURCES testLogger.cpp LIBS anch::log anch::ut)
anch_add_executable(anch-log-writers-test SOURCES testWriters.cpp LIBS anch::log anch::ut)

add_test(NAME anch-logger-rotate COMMAND anch-logger-test -t rotate)
add_test(NAME anch-logger-custom-file COMMAND anch-logger-test -t custom-file)
add_test(NAME anch-logger-low-priority COMMAND anch-logger-test -t low-priority)
add_test(NAME anch-logger-default-config COMMAND anch-logger-test -t default)
add_test(NAME anch-logger-bad-writers COMMAND anch-logger-test -t bad-writers)
add_test(NAME anch-logger-bad-loggers COMMAND anch-logger-test -t bad-loggers)
add_test(NAME anch-logger-writers-per-logger COMMAND anch-logger-test -t writers-per-logger)
add_test(NAME anch-logger-concurrent-get-logger COMMAND anch-logger-test -t concurrent-get-logger)
add_test(NAME anch-log-writers-default-pattern COMMAND anch-log-writers-test -t default-pattern)
add_test(NAME anch-log-writers-formatter COMMAND anch-log-writers-test -t formatter)
add_test(NAME anch-log-writers-mdc-formatter COMMAND anch-log-writers-test -t mdc-formatter)
add_test(NAME anch-log-writers-invalid-max-size COMMAND anch-log-writers-test -t invalid-max-size)
add_test(NAME anch-log-writers-rotation COMMAND anch-log-writers-test -t rotation)
add_test(NAME anch-log-writers-max-size-units COMMAND anch-log-writers-test -t max-size-units)
add_test(NAME anch-log-writers-no-archive COMMAND anch-log-writers-test -t no-archive)
add_test(NAME anch-log-writers-default-archives COMMAND anch-log-writers-test -t default-archives)
add_test(NAME anch-log-writers-low-priority-writer COMMAND anch-log-writers-test -t low-priority-writer)
add_test(NAME anch-log-writers-mdc COMMAND anch-log-writers-test -t mdc)

# Logger tests share the same log files: never run them in parallel
get_property(log_tests DIRECTORY PROPERTY TESTS)
set_tests_properties(${log_tests} PROPERTIES RESOURCE_LOCK anch-logger-files)
