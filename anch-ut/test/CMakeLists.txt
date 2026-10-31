set_property(DIRECTORY PROPERTY LABELS ut)

anch_add_executable(anch-unit-test SOURCES testUnit.cpp LIBS anch::ut)

add_test(NAME anch-unit-test COMMAND anch-unit-test)
add_test(NAME anch-unit-test-all COMMAND anch-unit-test -t all)
add_test(NAME anch-unit-test-nothing COMMAND anch-unit-test -t nothing)
add_test(NAME anch-unit-test-exception COMMAND anch-unit-test -t exception)
add_test(NAME anch-unit-test-assert COMMAND anch-unit-test -t assert)
add_test(NAME anch-unit-test-assert-format COMMAND anch-unit-test -t assert-format)
set_tests_properties(anch-unit-test PROPERTIES WILL_FAIL TRUE)
set_tests_properties(anch-unit-test-all PROPERTIES WILL_FAIL TRUE)
set_tests_properties(anch-unit-test-exception PROPERTIES WILL_FAIL TRUE)
set_tests_properties(anch-unit-test-assert PROPERTIES WILL_FAIL TRUE)
set_tests_properties(anch-unit-test-assert-format PROPERTIES WILL_FAIL TRUE)

# exit() called during a step has to be reported as a failure (T5) +
add_test(NAME anch-unit-test-exit-in-test COMMAND anch-unit-test -t exit-in-test)
add_test(NAME anch-unit-test-exit-in-test-message COMMAND anch-unit-test -t exit-in-test)
add_test(NAME anch-unit-test-exit-in-init COMMAND anch-unit-test -t nothing)
add_test(NAME anch-unit-test-exit-in-init-message COMMAND anch-unit-test -t nothing)
add_test(NAME anch-unit-test-exit-in-uninit COMMAND anch-unit-test -t nothing)
add_test(NAME anch-unit-test-exit-in-uninit-message COMMAND anch-unit-test -t nothing)
set_tests_properties(anch-unit-test-exit-in-test anch-unit-test-exit-in-init anch-unit-test-exit-in-uninit PROPERTIES WILL_FAIL TRUE)
set_tests_properties(anch-unit-test-exit-in-test-message PROPERTIES
  PASS_REGULAR_EXPRESSION "exit\\(\\) has been called during unit test exit-in-test: unit tests are failed")
set_tests_properties(anch-unit-test-exit-in-init anch-unit-test-exit-in-init-message PROPERTIES ENVIRONMENT "ANCH_UT_EXIT_IN_INIT=1")
set_tests_properties(anch-unit-test-exit-in-init-message PROPERTIES
  PASS_REGULAR_EXPRESSION "exit\\(\\) has been called during unit tests initialization: unit tests are failed")
set_tests_properties(anch-unit-test-exit-in-uninit anch-unit-test-exit-in-uninit-message PROPERTIES ENVIRONMENT "ANCH_UT_EXIT_IN_UNINIT=1")
set_tests_properties(anch-unit-test-exit-in-uninit-message PROPERTIES
  PASS_REGULAR_EXPRESSION "exit\\(\\) has been called during unit tests uninitialization: unit tests are failed")
# exit() called during a step has to be reported as a failure (T5) -
