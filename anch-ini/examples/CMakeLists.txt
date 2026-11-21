set_property(DIRECTORY PROPERTY LABELS ini)

anch_copy_test_files(test.ini)
anch_add_example(ini SOURCES ini.cpp LIBS anch::ini RUN)
