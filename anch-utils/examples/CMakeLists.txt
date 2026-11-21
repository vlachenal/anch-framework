set_property(DIRECTORY PROPERTY LABELS utils)

anch_add_example(utils SOURCES utils.cpp LIBS anch::utils Threads::Threads RUN)
anch_add_example(singleton SOURCES singleton.cpp LIBS anch::utils RUN)
