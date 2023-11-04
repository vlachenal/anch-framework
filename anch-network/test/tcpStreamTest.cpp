#include <iostream>
#include <thread>

#include "network/tcpStream.hpp"

class TcpStreamServer {
private:
  TcpSocket server;
public:
  void before();
  void after();
};

class TcpStreamClient {
private:
  TcpSocket client;
public:
  void before();
  void after();
};

void
TcpStreamTest::startup() {
  std::cout << "Create server TCP socket on port 40099" << std::endl;
  TcpStreamTest::server("",40099);
  std::cout << "bind()" << std::endl;
  server.bind();
  std::cout << "listen()" << std::endl;
  server.listen();
}

void
TcpStreamTest::before() {
  std::cout << "Create client TCP socket on localhost:40099" << std::endl;
  client("127.0.0.1",40099);
  std::cout << "connect()" << std::endl;
  client.connect();
}

void
TcpStreamTest::after() {
  std::cout << "After test" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));
  client.send("Bye bye !");
  client.shutdown();
}

void
TcpStreamTest::shutdown() {
  server.close();
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH network stream tests")
    .description("Test AnCH network stream tests library")
    .initialize(startup)
    .beforeTest(before)
    .afterTest(after)
    .uninitialize(shutdown)
    .add("nothing", testNothing)
    ;
}
