set_property(DIRECTORY PROPERTY LABELS date)

anch_add_executable(anch-date-format-test SOURCES testDateFormat.cpp LIBS anch::date anch::ut)

add_test(NAME date-format COMMAND anch-date-format-test -t date-format)
add_test(NAME date-format-time_t COMMAND anch-date-format-test -t date-format-time_t)
add_test(NAME date-parse COMMAND anch-date-format-test -t date-parse)
add_test(NAME date-comparison COMMAND anch-date-format-test -t date-comparison)
add_test(NAME date-format-custom COMMAND anch-date-format-test -t date-format-custom)

# Date computations and parsing (TZ is set to check daylight saving time) +
add_test(NAME date-dst COMMAND anch-date-format-test -t date-dst)
add_test(NAME date-timespec COMMAND anch-date-format-test -t date-timespec)
add_test(NAME date-conversions COMMAND anch-date-format-test -t date-conversions)
add_test(NAME date-year-2-digits COMMAND anch-date-format-test -t date-year-2-digits)
add_test(NAME date-hour-12 COMMAND anch-date-format-test -t date-hour-12)
add_test(NAME date-parse-reset COMMAND anch-date-format-test -t date-parse-reset)
add_test(NAME date-parse-milliseconds COMMAND anch-date-format-test -t date-parse-milliseconds)
add_test(NAME date-parse-invalid COMMAND anch-date-format-test -t date-parse-invalid)
add_test(NAME date-formatter-ownership COMMAND anch-date-format-test -t date-formatter-ownership)
add_test(NAME date-unknown-pattern COMMAND anch-date-format-test -t date-unknown-pattern)
add_test(NAME date-out-of-range COMMAND anch-date-format-test -t date-out-of-range)
set_tests_properties(
  date-dst date-timespec date-conversions date-year-2-digits date-hour-12
  date-parse-reset date-parse-milliseconds date-parse-invalid date-formatter-ownership date-unknown-pattern
  PROPERTIES ENVIRONMENT "TZ=Europe/Paris")
# Date computations and parsing (TZ is set to check daylight saving time) -

# RFC 3339 +
anch_add_executable(anch-date-rfc3339-test SOURCES testRfc3339.cpp LIBS anch::date anch::ut)

add_test(NAME rfc3339-format-utc COMMAND anch-date-rfc3339-test -t rfc3339-format-utc)
add_test(NAME rfc3339-format-local COMMAND anch-date-rfc3339-test -t rfc3339-format-local)
add_test(NAME rfc3339-parse COMMAND anch-date-rfc3339-test -t rfc3339-parse)
add_test(NAME rfc3339-leap-second COMMAND anch-date-rfc3339-test -t rfc3339-leap-second)
add_test(NAME rfc3339-parse-invalid COMMAND anch-date-rfc3339-test -t rfc3339-parse-invalid)
add_test(NAME rfc3339-date COMMAND anch-date-rfc3339-test -t rfc3339-date)
set_tests_properties(rfc3339-format-local PROPERTIES ENVIRONMENT "TZ=Europe/Paris")
# RFC 3339 -
