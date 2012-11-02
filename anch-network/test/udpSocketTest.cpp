#include <iostream>
#include <thread>

#include "network/udpSocket.hpp"
#include "events/observer.hpp"


using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::thread;

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
  virtual void notify(const SocketEvent& evt) const throw() {
    cout << "Server receive " + evt.getMessage() << endl;
    cout << "Server send response" << endl;
    try {
      _sock->send("Bye bye !", evt.getAddress());
    } catch(const IOException& e) {
      cerr << "Server " << e.what() << endl;
    }
  }

};

class ClientObs : public Observer<SocketEvent> {

public:
  ClientObs() {};
  ~ClientObs() {};

public:
  virtual void notify(const SocketEvent& evt) const throw() {
    cout << "Client receive " + evt.getMessage() << endl;
  }

};


void
runUdpServer(UdpSocket* const serverSock) {
  // Wait for client
  ServerObs srvObs(serverSock);
  serverSock->addObserver(srvObs);

  cout << "Server is waiting for a message" << endl;
  string message;
  try {
    serverSock->receive(message);

  } catch(const IOException& e) {
    cerr << "Server " << e.what() << endl;
  }
}


/**
 * UDP socket unitary tests
 */
int
main(void) {
  cout << "Enter in UDP sockets unitary test" << endl;

  // Server socket +
  // Open the server UDP socket for listening on <any>:40099
  cout << "Create server UDP socket on port 40099" << endl;
  UdpSocket serverSock("",40099);
  cout << "bind()" << endl;
  serverSock.bind();
  thread srvTh(runUdpServer, &serverSock);
  // Server socket -

  std::this_thread::sleep_for(std::chrono::seconds(1));

  // Client socket +
  // Open the server UDP socket on localhost:40099
  cout << "Create client UDP socket on localhost:40099" << endl;
  UdpSocket clientSocket("127.0.0.1",40099);
  ClientObs cliObs;
  clientSocket.addObserver(cliObs);
  cout << "Client send a message" << endl;
  clientSocket.send("Hello !");

  cout << "Client is waiting for response" << endl;
  string message;
  clientSocket.receive(message);
  // Client socket -

  srvTh.join();

  clientSocket.close();
  serverSock.close();

  cout << "Exit UDP sockets unitary test" << endl;
  return 0;
}
