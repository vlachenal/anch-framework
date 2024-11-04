#include <iostream>
#include <thread>

#include "network/tcpStream.hpp"
#include "ut/unit.hpp"
#include "ut/assert.hpp"

using anch::network::TcpStreamServer;
using anch::network::TcpStreamClient;
using anch::network::SocketEvent;

TcpStreamServer* server;
TcpStreamClient* client;

class STDoutObs: public anch::events::Observer<SocketEvent> {
public:
  virtual void handle(const anch::events::Event<SocketEvent>& event) noexcept {
    std::cout << "Receive message: " << event.body.getMessage() << std::endl;
  }
};

class STDinObs: public anch::events::Observer<SocketEvent> {
public:
  virtual void handle(const anch::events::Event<SocketEvent>& event) noexcept {
    std::cout << "Send message: " << event.body.getMessage();
  }
};

STDinObs bos;
STDoutObs obs;

void
startup() {
  std::cout << "Create server TCP socket on port 40099" << std::endl;
  server = new TcpStreamServer("localhost",40099);
  server->addObserver(obs);
  // \todo manage server connection ... \see runTcpServer function in tcpSocketTest.cpp
}

void
before() {
  std::cout << "Create client TCP socket on localhost:40099" << std::endl;
  client = new TcpStreamClient("localhost",40099);
  client->addObserver(bos);
}

void
after() {
  std::cout << "After test" << std::endl;
  if(client != NULL) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    *client << "Bye bye !" << std::endl;
    std::string plop; // \todo remove
    *server >> plop;
    std::cout << "plop: " << plop << std::endl;
    delete client;
  }
}

void
uninit() {
  std::cout << "Unitialize" << std::endl;
  if(server != NULL) {
    std::string plop; // \todo remove
    *server >> plop;
    std::cout << "plop: " << plop << std::endl;
    delete server;
  }
  if(client != NULL) {
    delete client;
  }
}

void
testNothing() {
  //std::cout << "plop" << std::endl;
  *client << "Nothing" << std::endl;
  //server->receive();
  //std::cout << server->rdbuf() << std::endl;
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH network stream tests")
    .description("Test AnCH network stream tests library")
    .initialize(startup)
    .beforeTest(before)
    .afterTest(after)
    .uninitialize(uninit)
    .add("nothing", testNothing)
    ;
}
