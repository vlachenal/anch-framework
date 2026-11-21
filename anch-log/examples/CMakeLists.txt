set_property(DIRECTORY PROPERTY LABELS log)

anch_copy_test_files(application.ini)
anch_add_example(log SOURCES log.cpp LIBS anch::log anch::conf RUN)
