set_property(DIRECTORY PROPERTY LABELS csv)

anch_add_executable(anch-csv-test SOURCES testCsv.cpp LIBS anch::csv anch::ut)

add_test(NAME anch-csv-rfc4180 COMMAND anch-csv-test -t rfc4180)
add_test(NAME anch-csv-line-endings COMMAND anch-csv-test -t line-endings)
add_test(NAME anch-csv-empty-fields COMMAND anch-csv-test -t empty-fields)
add_test(NAME anch-csv-bom COMMAND anch-csv-test -t bom)
add_test(NAME anch-csv-options COMMAND anch-csv-test -t options)
add_test(NAME anch-csv-errors COMMAND anch-csv-test -t errors)
add_test(NAME anch-csv-limits COMMAND anch-csv-test -t limits)
add_test(NAME anch-csv-writer COMMAND anch-csv-test -t writer)
add_test(NAME anch-csv-round-trip COMMAND anch-csv-test -t round-trip)
add_test(NAME anch-csv-stream COMMAND anch-csv-test -t stream)
