set_property(DIRECTORY PROPERTY LABELS network)

anch_add_executable(anch-network-test SOURCES testNetwork.cpp LIBS anch::network anch::ut)

add_test(NAME anch-network-tcp-echo COMMAND anch-network-test -t tcp-echo)
add_test(NAME anch-network-tcp-large COMMAND anch-network-test -t tcp-large)
add_test(NAME anch-network-tcp-eof COMMAND anch-network-test -t tcp-eof)
add_test(NAME anch-network-tcp-ipv6 COMMAND anch-network-test -t tcp-ipv6)
add_test(NAME anch-network-tcp-refused COMMAND anch-network-test -t tcp-refused)
add_test(NAME anch-network-tcp-receive-timeout COMMAND anch-network-test -t tcp-receive-timeout)
add_test(NAME anch-network-socket-ownership COMMAND anch-network-test -t socket-ownership)
add_test(NAME anch-network-udp COMMAND anch-network-test -t udp)
add_test(NAME anch-network-udp-truncated COMMAND anch-network-test -t udp-truncated)
