set_property(DIRECTORY PROPERTY LABELS network)

anch_add_executable(anch-network-test-tcp SOURCES tcpSocketTest.cpp LIBS anch::network)
anch_add_executable(anch-network-test-udp SOURCES udpSocketTest.cpp LIBS anch::network)
anch_add_executable(anch-network-test-tcp-stream SOURCES tcpStreamTest.cpp LIBS anch::network anch::ut)

add_test(NAME anch-network-test-tcp COMMAND anch-network-test-tcp main)
add_test(NAME anch-network-test-udp COMMAND anch-network-test-udp main)
#add_test(NAME anch-network-test-tcp-stream COMMAND anch-network-test-tcp-stream)
