set_property(DIRECTORY PROPERTY LABELS network)

# Echo server and client on the loopback interface (free port): no external network service
anch_add_example(network SOURCES network.cpp LIBS anch::network Threads::Threads RUN)
# Fragments of tcpServer.hpp and tcpStream.hpp documentation: fixed port 40099 on the loopback interface
anch_add_example(network-fragments SOURCES fragments.cpp LIBS anch::network RUN)
