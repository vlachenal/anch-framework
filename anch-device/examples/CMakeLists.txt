set_property(DIRECTORY PROPERTY LABELS device)

anch_add_example(device SOURCES device.cpp LIBS anch::device RUN)
