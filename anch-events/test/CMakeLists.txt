set_property(DIRECTORY PROPERTY LABELS events)

anch_add_executable(anch-observ-test SOURCES testEvents.cpp LIBS anch::events)
anch_add_executable(anch-handlers-test SOURCES testMultiHandler.cpp LIBS anch::events)
anch_add_executable(anch-event-bus-test SOURCES testEventBus.cpp LIBS anch::events)

add_test(NAME anch-observ-test COMMAND anch-observ-test main)
add_test(NAME anch-handlers-test COMMAND anch-handlers-test main)
add_test(NAME anch-event-bus-test COMMAND anch-event-bus-test main)
