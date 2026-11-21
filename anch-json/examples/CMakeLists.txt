set_property(DIRECTORY PROPERTY LABELS json)

anch_add_example(json SOURCES json.cpp LIBS anch::json RUN)
anch_add_example(json-required SOURCES required.cpp LIBS anch::json RUN)
anch_add_example(json-constraints SOURCES constraints.cpp LIBS anch::json RUN)
anch_add_example(json-validation SOURCES validation.cpp LIBS anch::json RUN)
anch_add_example(json-fragments SOURCES fragments.cpp LIBS anch::json RUN)
