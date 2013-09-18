#include <iostream>
#include <thread>

#include "network/tcpSocket.hpp"
#include "events/observer.hpp"


using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::thread;

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
  virtual void notify(const SocketEvent& evt) noexcept {
    cout << "Server receive " + evt.getMessage() << endl;
  }

};

class ClientObs : public Observer<SocketEvent> {

public:
  ClientObs() {};
  ~ClientObs() {};

public:
  virtual void notify(const SocketEvent& evt) noexcept {
    cout << "Client receive " + evt.getMessage() << endl;
  }

};


void
runTcpServer(TcpSocket* const serverSock) {
  cout << "Server: Wait for client connection" << endl;
  // Wait for client
  TcpSocket csock;
  serverSock->accept(csock);
  ServerObs srvObs;
  csock.addObserver(srvObs);

  std::this_thread::sleep_for(std::chrono::seconds(1));

  cout << "Server send a message" << endl;
  try {
    csock.send("You are connected !!");
  } catch(const IOException& e) {
    cerr << "Server " << e.what() << endl;
  }
  csock.shutdown(Direction::TRANSMISSION);

  cout << "Server is waiting for a message" << endl;
  string message;
  try {
    csock.receive(message);
  } catch(const IOException& e) {
    cerr << "Server " << e.what() << endl;
  }
  csock.close();
}


/**
 * TCP socket unitary tests
 */
int
main(void) {
  cout << "Enter in TCP sockets unitary test" << endl;

  // Server socket +
  // Open the server TCP socket for listening on <any>:40099
  cout << "Create server TCP socket on port 40099" << endl;
  TcpSocket serverSock("",40099);
  cout << "bind()" << endl;
  serverSock.bind();
  cout << "listen()" << endl;
  serverSock.listen();
  thread srvTh(runTcpServer, &serverSock);
  // Server socket -

  // Client socket +
  // Open the server TCP socket on localhost:40099
  cout << "Create client TCP socket on localhost:40099" << endl;
  TcpSocket clientSocket("127.0.0.1",40099);
  ClientObs cliObs;
  clientSocket.addObserver(cliObs);
  cout << "connect()" << endl;
  try {
    clientSocket.connect();
  } catch(const IOException& e) {
    cerr << "Client " << e.what() << endl;
    serverSock.close();
  }
  cout << "Client is waiting for message" << endl;
  string message;
  clientSocket.receive(message);

  std::this_thread::sleep_for(std::chrono::seconds(1));

  cout << "Client send a message" << endl;
  clientSocket.send("Bye bye !");
  clientSocket.shutdown();
  // Client socket -

  srvTh.join();

  clientSocket.close();
  serverSock.close();

  cout << "Exit TCP sockets unitary test" << endl;
  return 0;
}
