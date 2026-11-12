set_property(DIRECTORY PROPERTY LABELS conf)

anch_copy_test_files(
  test.ini
  test-UT.conf
  test-inc.conf
  test-defprof.ini
  test-defprof-UT.conf
  test-placeholders.ini
  cascade.ini
  cascade-inc.ini
  cascade-db.ini
  cascade-p1.ini
  cascade-p2.ini
  cascade-inc-p1.ini)

set(test_libs anch::conf anch::ut)
anch_add_executable(anch-conf-parse SOURCES testConf.cpp LIBS ${test_libs})
anch_add_executable(anch-conf-profile-parse SOURCES testConfProfile.cpp LIBS ${test_libs})
anch_add_executable(anch-conf-default-profile-parse SOURCES testDefaultProfiles.cpp LIBS ${test_libs})
anch_add_executable(anch-conf-pattern-test SOURCES testPattern.cpp LIBS ${test_libs})
anch_add_executable(anch-conf-placeholders-test SOURCES testPlaceHolders.cpp LIBS ${test_libs})
anch_add_executable(anch-conf-resolution-test SOURCES testResolution.cpp LIBS ${test_libs})
anch_add_executable(anch-conf-cascade-test SOURCES testCascade.cpp LIBS ${test_libs})

add_test(NAME anch-conf-get-TOTO-toto COMMAND anch-conf-parse -t get-TOTO-toto)
add_test(NAME anch-conf-get-tata COMMAND anch-conf-parse -t get-tata)
add_test(NAME anch-conf-get-toto COMMAND anch-conf-parse -t get-toto)
add_test(NAME anch-conf-prof-get-TOTO-toto COMMAND anch-conf-profile-parse -t get-TOTO-toto)
add_test(NAME anch-conf-prof-get-tata COMMAND anch-conf-profile-parse -t get-tata)
add_test(NAME anch-conf-prof-get-toto COMMAND anch-conf-profile-parse -t get-toto)
add_test(NAME anch-conf-defprof-get-TOTO-toto COMMAND anch-conf-default-profile-parse -t get-TOTO-toto)
add_test(NAME anch-conf-defprof-get-tata COMMAND anch-conf-default-profile-parse -t get-tata)
add_test(NAME anch-conf-defprof-get-toto COMMAND anch-conf-default-profile-parse -t get-toto)
add_test(NAME anch-conf-pattern-check-conf COMMAND anch-conf-pattern-test -t check-conf)
add_test(NAME anch-conf-pattern-check-conf-default COMMAND anch-conf-pattern-test -t check-conf-default)
add_test(NAME anch-conf-pattern-check-env-conf-default COMMAND anch-conf-pattern-test -t check-env-conf-default)
add_test(NAME anch-conf-placeholders-check-conf COMMAND anch-conf-placeholders-test -t single)
add_test(NAME anch-conf-placeholders-alternatives COMMAND anch-conf-placeholders-test -t alternatives)
add_test(NAME anch-conf-placeholders-loaded COMMAND anch-conf-placeholders-test -t loaded)
add_test(NAME anch-conf-resolution-alternatives COMMAND anch-conf-resolution-test -t alternatives)
add_test(NAME anch-conf-resolution-propagation COMMAND anch-conf-resolution-test -t propagation)
add_test(NAME anch-conf-resolution-missing COMMAND anch-conf-resolution-test -t missing)
add_test(NAME anch-conf-resolution-cycle COMMAND anch-conf-resolution-test -t cycle)
add_test(NAME anch-conf-resolution-watchdog COMMAND anch-conf-resolution-test -t watchdog)
add_test(NAME anch-conf-resolution-literal COMMAND anch-conf-resolution-test -t literal)
add_test(NAME anch-conf-resolution-syntax COMMAND anch-conf-resolution-test -t syntax)
add_test(NAME anch-conf-cascade-includes COMMAND anch-conf-cascade-test -t includes)
add_test(NAME anch-conf-cascade-profiles COMMAND anch-conf-cascade-test -t profiles)
add_test(NAME anch-conf-cascade-values COMMAND anch-conf-cascade-test -t values)
add_test(NAME anch-conf-cascade-section COMMAND anch-conf-cascade-test -t section)

