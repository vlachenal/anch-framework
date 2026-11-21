set_property(DIRECTORY PROPERTY LABELS ut)

# anch-ut provides main(): examples are test suites ('exception', 'assert' and 'assert-format' fail on purpose)
anch_add_example(ut-suite SOURCES suite.cpp LIBS anch::ut RUN ARGS -t nothing)
anch_add_example(ut-thread-errors SOURCES threadErrors.cpp LIBS anch::ut Threads::Threads RUN)
