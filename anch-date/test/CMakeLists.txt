set_property(DIRECTORY PROPERTY LABELS date)

anch_add_executable(anch-date-format-test SOURCES testDateFormat.cpp LIBS anch::date anch::ut)

add_test(NAME date-format COMMAND anch-date-format-test -t date-format)
add_test(NAME date-format-time_t COMMAND anch-date-format-test -t date-format-time_t)
add_test(NAME date-parse COMMAND anch-date-format-test -t date-parse)
add_test(NAME date-comparison COMMAND anch-date-format-test -t date-comparison)
add_test(NAME date-format-custom COMMAND anch-date-format-test -t date-format-custom)
