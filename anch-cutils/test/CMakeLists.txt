set_property(DIRECTORY PROPERTY LABELS cutils)

anch_add_executable(anch-cstream-test SOURCES testCStream.cpp
  LIBS anch::cutils anch::json anch::ut)

add_test(NAME cstream-output-jsonser COMMAND anch-cstream-test --test cstream-output-jsonser)
add_test(NAME cstream-output-jsondeser COMMAND anch-cstream-test --test cstream-output-jsondeser)
add_test(NAME cstream-output-jsonser-1char COMMAND anch-cstream-test --test cstream-output-jsonser-1char)
add_test(NAME cstream-output-jsondeser-1char COMMAND anch-cstream-test --test cstream-output-jsondeser-1char)
add_test(NAME cstream-output-jsonser-1500char COMMAND anch-cstream-test --test cstream-output-jsonser-1500char)
add_test(NAME cstream-output-jsondeser-1500char COMMAND anch-cstream-test --test cstream-output-jsondeser-1500char)
add_test(NAME cstream-output-jsonser-25char COMMAND anch-cstream-test --test cstream-output-jsonser-25char)
add_test(NAME cstream-output-jsondeser-25char COMMAND anch-cstream-test --test cstream-output-jsondeser-25char)
add_test(NAME cstream-inout-json-1500char COMMAND anch-cstream-test --test cstream-inout-json-1500char)
add_test(NAME cstream-inout-json-1char COMMAND anch-cstream-test --test cstream-inout-json-1char)
add_test(NAME cstream-inout-json-4char COMMAND anch-cstream-test --test cstream-inout-json-4char)
