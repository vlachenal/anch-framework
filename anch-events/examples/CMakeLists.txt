set_property(DIRECTORY PROPERTY LABELS events)

anch_add_example(events SOURCES events.cpp LIBS anch::events Threads::Threads RUN)
