set_property(DIRECTORY PROPERTY LABELS device)

set(test_libs anch::device anch::uuid)
anch_add_executable(anch-device-valid-network-interface SOURCES testValidNetworkInterface.cpp LIBS ${test_libs})
anch_add_executable(anch-device-invalid-network-interface SOURCES testInvalidNetworkInterface.cpp LIBS ${test_libs})
anch_add_executable(anch-device-cpu SOURCES testCPU.cpp LIBS ${test_libs})
anch_add_executable(anch-device-uuid SOURCES testUuid.cpp LIBS ${test_libs})

add_test(NAME anch-device-valid-network-interface COMMAND anch-device-valid-network-interface main)
add_test(NAME anch-device-invalid-network-interface COMMAND anch-device-invalid-network-interface main)
add_test(NAME anch-device-cpu COMMAND anch-device-cpu main)
add_test(NAME anch-device-uuid COMMAND anch-device-uuid main)
