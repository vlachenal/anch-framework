set_property(DIRECTORY PROPERTY LABELS crypto)

anch_add_example(crypto SOURCES crypto.cpp LIBS anch::crypto RUN)
