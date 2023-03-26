#include <iostream>
#include <thread>

#include "network/udpSocket.hpp"
#include "events/observer.hpp"


using anch::network::UdpSocket;
using anch::network::SocketEvent;
using anch::network::IOException;
using anch::network::Direction;

using anch::events::Observer;


class ServerObs : public Observer<SocketEvent> {

private:
  UdpSocket* _sock;

public:
  ServerObs(UdpSocket* sock): _sock(sock) {};
  ~ServerObs() {};

public:
  virtual void handle(const anch::events::Event<SocketEvent>& evt) noexcept override {
    std::cout << "Server receive " + evt.body.getMessage() << std::endl;
    std::cout << "Server send response" << std::endl;
    try {
      _sock->send("Bye bye !", evt.body.getAddress());
    } catch(const IOException& e) {
      std::cerr << "Server " << e.what() << std::endl;
    }
  }

};

class ClientObs : public Observer<SocketEvent> {

public:
  ClientObs() {};
  ~ClientObs() {};

public:
  virtual void handle(const anch::events::Event<SocketEvent>& evt) noexcept override {
    std::cout << "Client receive " + evt.body.getMessage() << std::endl;
  }

};


void
runUdpServer(UdpSocket* const serverSock) {
  // Wait for client
  ServerObs srvObs(serverSock);
  serverSock->addObserver(srvObs);

  std::cout << "Server is waiting for a message" << std::endl;
  std::string message;
  try {
    serverSock->receive(message);

  } catch(const IOException& e) {
    std::cerr << "Server " << e.what() << std::endl;
  }
}


/*!
 * UDP socket unitary tests
 */
int
main(void) {
  std::cout << "Enter in UDP sockets unitary test" << std::endl;

  // Server socket +
  // Open the server UDP socket for listening on <any>:40099
  std::cout << "Create server UDP socket on port 40099" << std::endl;
  UdpSocket serverSock("",40099);
  std::cout << "bind()" << std::endl;
  serverSock.bind();
  std::thread srvTh(runUdpServer, &serverSock);
  // Server socket -

  std::this_thread::sleep_for(std::chrono::seconds(1));

  // Client socket +
  // Open the server UDP socket on localhost:40099
  std::cout << "Create client UDP socket on localhost:40099" << std::endl;
  UdpSocket clientSocket("127.0.0.1",40099);
  ClientObs cliObs;
  clientSocket.addObserver(cliObs);
  std::cout << "Client send a message" << std::endl;
  clientSocket.send("Hello !");

  std::cout << "Client is waiting for response" << std::endl;
  std::string message;
  clientSocket.receive(message);
  // Client socket -

  srvTh.join();

  clientSocket.close();
  serverSock.close();

  std::cout << "Exit UDP sockets unitary test" << std::endl;
  return 0;
}
