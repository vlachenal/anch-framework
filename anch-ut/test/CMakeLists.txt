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

# Failed assertion location is reported (T9) +
add_test(NAME anch-unit-test-assert-location COMMAND anch-unit-test -t assert-location)
add_test(NAME anch-unit-test-assert-location-message COMMAND anch-unit-test -t assert-location)
add_test(NAME anch-unit-test-assert-true-location COMMAND anch-unit-test -t assert-true-location)
add_test(NAME anch-unit-test-assert-true-location-message COMMAND anch-unit-test -t assert-true-location)
set_tests_properties(anch-unit-test-assert-location anch-unit-test-assert-true-location PROPERTIES WILL_FAIL TRUE)
set_tests_properties(anch-unit-test-assert-location-message PROPERTIES
  PASS_REGULAR_EXPRESSION "failed on assertion: located assertion 42\n  at [^\n]*testUnit\\.cpp:[0-9]+ in [^\n]*testAssertLocation")
set_tests_properties(anch-unit-test-assert-true-location-message PROPERTIES
  PASS_REGULAR_EXPRESSION "failed on assertion: located assertTrue\n  at [^\n]*testUnit\\.cpp:[0-9]+ in [^\n]*testAssertTrueLocation")
# Failed assertion location is reported (T9) -

# Assertion failed in another thread is reported (T10) +
add_test(NAME anch-unit-test-thread-assert COMMAND anch-unit-test -t thread-assert)
add_test(NAME anch-unit-test-thread-assert-message COMMAND anch-unit-test -t thread-assert)
add_test(NAME anch-unit-test-thread-ok COMMAND anch-unit-test -t thread-ok)
set_tests_properties(anch-unit-test-thread-assert PROPERTIES WILL_FAIL TRUE)
set_tests_properties(anch-unit-test-thread-assert-message PROPERTIES
  PASS_REGULAR_EXPRESSION "Unit test thread-assert execution failed on assertion: assertion failed in thread 2")
# Assertion failed in another thread is reported (T10) -

# Exception during setup is reported (T11) +
add_test(NAME anch-unit-test-setup-error COMMAND anch-unit-test -t nothing)
add_test(NAME anch-unit-test-setup-error-message COMMAND anch-unit-test -t nothing)
set_tests_properties(anch-unit-test-setup-error anch-unit-test-setup-error-message PROPERTIES ENVIRONMENT "ANCH_UT_DUPLICATE=1")
set_tests_properties(anch-unit-test-setup-error PROPERTIES WILL_FAIL TRUE)
set_tests_properties(anch-unit-test-setup-error-message PROPERTIES
  PASS_REGULAR_EXPRESSION "Unit tests setup failed: Test nothing has already been registered")
# Exception during setup is reported (T11) -

# Uninitialization is not called when initialization fails ; uninitialization failure fails the suite (T12) +
add_test(NAME anch-unit-test-init-error COMMAND anch-unit-test -t nothing)
add_test(NAME anch-unit-test-init-error-message COMMAND anch-unit-test -t nothing)
set_tests_properties(anch-unit-test-init-error anch-unit-test-init-error-message PROPERTIES ENVIRONMENT "ANCH_UT_FAIL_IN_INIT=1")
set_tests_properties(anch-unit-test-init-error PROPERTIES WILL_FAIL TRUE)
set_tests_properties(anch-unit-test-init-error-message PROPERTIES
  PASS_REGULAR_EXPRESSION "Unit tests initialization failed on assertion: initialization failure"
  FAIL_REGULAR_EXPRESSION "Nothing;Uninitialize tests suite")
add_test(NAME anch-unit-test-uninit-error COMMAND anch-unit-test -t nothing)
set_tests_properties(anch-unit-test-uninit-error PROPERTIES ENVIRONMENT "ANCH_UT_FAIL_IN_UNINIT=1" WILL_FAIL TRUE)
# Uninitialization is not called when initialization fails ; uninitialization failure fails the suite (T12) -

# Tests are run in registration order, or in command line order (T13) +
add_test(NAME anch-unit-test-list-order COMMAND anch-unit-test -l)
set_tests_properties(anch-unit-test-list-order PROPERTIES PASS_REGULAR_EXPRESSION "z-registered-first.*a-registered-second")
add_test(NAME anch-unit-test-cli-order COMMAND anch-unit-test -t a-registered-second -t z-registered-first -t a-registered-second)
set_tests_properties(anch-unit-test-cli-order PROPERTIES
  PASS_REGULAR_EXPRESSION "Run a-registered-second.*Run z-registered-first"
  FAIL_REGULAR_EXPRESSION "Run z-registered-first.*Run a-registered-second")
# Tests are run in registration order, or in command line order (T13) -

# Equality helpers (T14, T17) +
add_test(NAME anch-unit-test-equals-ptr COMMAND anch-unit-test -t equals-ptr)
add_test(NAME anch-unit-test-equals-collections COMMAND anch-unit-test -t equals-collections)
# Equality helpers (T14, T17) -

# Not implemented options (removed, UT1) are refused (T3, T4) +
add_test(NAME anch-unit-test-runner COMMAND anch-unit-test -r tcp://localhost:54321)
add_test(NAME anch-unit-test-gen-ctest COMMAND anch-unit-test --gen-ctest)
set_tests_properties(anch-unit-test-runner anch-unit-test-gen-ctest PROPERTIES WILL_FAIL TRUE)
add_test(NAME anch-unit-test-runner-message COMMAND anch-unit-test -r tcp://localhost:54321)
set_tests_properties(anch-unit-test-runner-message PROPERTIES PASS_REGULAR_EXPRESSION "Unknown argument -r")
# Not implemented options (removed, UT1) are refused (T3, T4) -

# No ANSI escape sequence when output is not a terminal (T16) +
string(ASCII 27 _anch_esc)
add_test(NAME anch-unit-test-no-color COMMAND anch-unit-test -t assert)
set_tests_properties(anch-unit-test-no-color PROPERTIES
  PASS_REGULAR_EXPRESSION "execution failed on assertion"
  FAIL_REGULAR_EXPRESSION "${_anch_esc}")
# No ANSI escape sequence when output is not a terminal (T16) -
