set_property(DIRECTORY PROPERTY LABELS conf)

anch_copy_test_files(application.ini application-dev.ini database.ini security.ini)
anch_add_example(conf SOURCES conf.cpp LIBS anch::conf RUN)
