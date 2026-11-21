set_property(DIRECTORY PROPERTY LABELS cli)

anch_add_example(cli SOURCES cli.cpp LIBS anch::cli RUN ARGS -v --plop value first second third fourth)
