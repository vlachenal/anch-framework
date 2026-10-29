set_property(DIRECTORY PROPERTY LABELS conf)

anch_copy_test_files(
  test.ini
  test-UT.conf
  test-inc.conf
  test-defprof.ini
  test-defprof-UT.conf
  test-placeholders.ini)

set(test_libs anch::conf anch::ut)
anch_add_executable(anch-conf-parse SOURCES testConf.cpp LIBS ${test_libs})
anch_add_executable(anch-conf-profile-parse SOURCES testConfProfile.cpp LIBS ${test_libs})
anch_add_executable(anch-conf-default-profile-parse SOURCES testDefaultProfiles.cpp LIBS ${test_libs})
anch_add_executable(anch-conf-pattern-test SOURCES testPattern.cpp LIBS ${test_libs})
anch_add_executable(anch-conf-placeholders-test SOURCES testPlaceHolders.cpp LIBS ${test_libs})

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
