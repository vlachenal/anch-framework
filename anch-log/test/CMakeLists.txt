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
anch_add_executable(anch-log-stream-test SOURCES testStream.cpp LIBS anch::log anch::ut)
# Same tests with TRACE and DEBUG levels removed at compile time.
# Only with default ANCH_LOG_MIN_LEVEL: otherwise anch::log already defines it and anch-log-stream-test covers it
if(log_min_level STREQUAL "TRACE")
  anch_add_executable(anch-log-stream-min-level-test SOURCES testStream.cpp LIBS anch::log anch::ut)
  target_compile_definitions(anch-log-stream-min-level-test PRIVATE ANCH_LOG_MIN_LEVEL=INFO)
endif()

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
add_test(NAME anch-log-stream-runtime-level COMMAND anch-log-stream-test -t runtime-level)
add_test(NAME anch-log-stream-manipulators COMMAND anch-log-stream-test -t manipulators)
add_test(NAME anch-log-stream-format-error COMMAND anch-log-stream-test -t format-error)
add_test(NAME anch-log-stream-writer-error COMMAND anch-log-stream-test -t writer-error)
add_test(NAME anch-log-stream-evaluation COMMAND anch-log-stream-test -t evaluation)
add_test(NAME anch-log-stream-compile-level COMMAND anch-log-stream-test -t compile-level)
add_test(NAME anch-log-stream-variadic-compile-level COMMAND anch-log-stream-test -t variadic-compile-level)
if(log_min_level STREQUAL "TRACE")
  add_test(NAME anch-log-stream-min-level-runtime-level COMMAND anch-log-stream-min-level-test -t runtime-level)
  add_test(NAME anch-log-stream-min-level-manipulators COMMAND anch-log-stream-min-level-test -t manipulators)
  add_test(NAME anch-log-stream-min-level-format-error COMMAND anch-log-stream-min-level-test -t format-error)
  add_test(NAME anch-log-stream-min-level-writer-error COMMAND anch-log-stream-min-level-test -t writer-error)
  add_test(NAME anch-log-stream-min-level-evaluation COMMAND anch-log-stream-min-level-test -t evaluation)
  add_test(NAME anch-log-stream-min-level-compile-level COMMAND anch-log-stream-min-level-test -t compile-level)
  add_test(NAME anch-log-stream-min-level-variadic-compile-level COMMAND anch-log-stream-min-level-test -t variadic-compile-level)
endif()

# Logger tests share the same log files: never run them in parallel
get_property(log_tests DIRECTORY PROPERTY TESTS)
set_tests_properties(${log_tests} PROPERTIES RESOURCE_LOCK anch-logger-files)
