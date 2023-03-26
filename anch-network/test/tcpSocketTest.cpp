#include <iostream>
#include <thread>

#include "network/tcpSocket.hpp"
#include "events/observer.hpp"
#include "events/event.hpp"


using anch::network::TcpSocket;
using anch::network::SocketEvent;
using anch::network::IOException;
using anch::network::Direction;

using anch::events::Observer;


class ServerObs : public Observer<SocketEvent> {

public:
  ServerObs() {};
  ~ServerObs() {};

public:
  virtual void handle(const anch::events::Event<SocketEvent>& evt) noexcept override {
    std::cout << "Server receive " + evt.body.getMessage() << std::endl;
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
runTcpServer(TcpSocket* const serverSock) {
  std::cout << "Server: Wait for client connection" << std::endl;
  // Wait for client
  TcpSocket csock;
  serverSock->accept(csock);
  ServerObs srvObs;
  csock.addObserver(srvObs);

  std::this_thread::sleep_for(std::chrono::seconds(1));

  std::cout << "Server send a message" << std::endl;
  try {
    csock.send("You are connected !!");
  } catch(const IOException& e) {
    std::cerr << "Server " << e.what() << std::endl;
  }
  csock.shutdown(Direction::TRANSMISSION);

  std::cout << "Server is waiting for a message" << std::endl;
  std::string message;
  try {
    csock.receive(message);
  } catch(const IOException& e) {
    std::cerr << "Server " << e.what() << std::endl;
  }
  csock.close();
}


/*!
 * TCP socket unitary tests
 */
int
main(void) {
  std::cout << "Enter in TCP sockets unitary test" << std::endl;

  // Server socket +
  // Open the server TCP socket for listening on <any>:40099
  std::cout << "Create server TCP socket on port 40099" << std::endl;
  TcpSocket serverSock("",40099);
  std::cout << "bind()" << std::endl;
  serverSock.bind();
  std::cout << "listen()" << std::endl;
  serverSock.listen();
  std::thread srvTh(runTcpServer, &serverSock);
  // Server socket -

  // Client socket +
  // Open the server TCP socket on localhost:40099
  std::cout << "Create client TCP socket on localhost:40099" << std::endl;
  TcpSocket clientSocket("127.0.0.1",40099);
  ClientObs cliObs;
  clientSocket.addObserver(cliObs);
  std::cout << "connect()" << std::endl;
  try {
    clientSocket.connect();
  } catch(const IOException& e) {
    std::cerr << "Client " << e.what() << std::endl;
    serverSock.close();
  }
  std::cout << "Client is waiting for message" << std::endl;
  std::string message;
  clientSocket.receive(message);

  std::this_thread::sleep_for(std::chrono::seconds(1));

  std::cout << "Client send a message" << std::endl;
  clientSocket.send("Bye bye !");
  clientSocket.shutdown();
  // Client socket -

  srvTh.join();

  clientSocket.close();
  serverSock.close();

  std::cout << "Exit TCP sockets unitary test" << std::endl;
  return 0;
}
