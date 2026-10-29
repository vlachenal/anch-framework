set_property(DIRECTORY PROPERTY LABELS uuid)

anch_add_executable(anch-uuid-test SOURCES testUuid.cpp LIBS anch::uuid anch::ut)

add_test(NAME uuid-random COMMAND anch-uuid-test -t uuid-random)
add_test(NAME uuid-random-time COMMAND anch-uuid-test -t uuid-random-time)
add_test(NAME uuid-collision COMMAND anch-uuid-test -t uuid-collision)
add_test(NAME uuid-perf COMMAND anch-uuid-test -t uuid-perf)
