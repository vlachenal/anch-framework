/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2026 Vincent Lachenal

  This file is part of ANCH Framework.

  ANCH Framework is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ANCH Framework is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "network/socket.hpp"
#include "network/tcpServer.hpp"
#include "network/tcpStream.hpp"
#include "network/udpSocket.hpp"

#include <cerrno>
#include <chrono>
#include <string>
#include <thread>

#include <fcntl.h>

#include "ut/assert.hpp"
#include "ut/threadErrors.hpp"
#include "ut/unit.hpp"

using anch::network::Address;
using anch::network::Direction;
using anch::network::IOException;
using anch::network::Socket;
using anch::network::SocketType;
using anch::network::TcpServer;
using anch::network::TcpStream;
using anch::network::UdpSocket;

using namespace std::chrono_literals;

/*!
 * Timeout of every blocking reception: a broken test fails instead of hanging.\n
 * It is a safety net only: tests run under sanitizers on loaded CI machines.
 */
constexpr auto TIMEOUT = 30s;

/*!
 * Run server side in a thread: accept one connection and call handler with the stream
 */
template<typename F>
std::thread
serve(TcpServer& server, anch::ut::ThreadErrors& errors, F handler) {
  return std::thread(errors.wrap([&server, handler]() mutable {
    TcpStream peer = server.accept();
    peer.getSocket().setReceiveTimeout(TIMEOUT);
    handler(peer);
  }));
}

void
testTcpEcho() {
  TcpServer server("127.0.0.1", 0);
  uint16_t port = server.getAddress().getPort();
  anch::ut::assertTrue(port != 0, "Ephemeral port should be set");
  anch::ut::ThreadErrors errors;
  std::thread srv = serve(server, errors, [](TcpStream& peer) {
    std::string line;
    while(std::getline(peer, line)) {
      peer << "echo: " << line << '\n'; // flushed before next read
    }
  });
  {
    TcpStream client("127.0.0.1", port);
    client.getSocket().setReceiveTimeout(TIMEOUT);
    std::string line;
    for(const char* msg : {"hello", "world", ""}) {
      client << msg << std::endl;
      anch::ut::assertTrue(static_cast<bool>(std::getline(client, line)), "Response expected");
      anch::ut::assertThat(line == std::string("echo: ") + msg, "Response is '{}'", line);
    }
  }
  srv.join();
  errors.rethrow();
}

void
testTcpLarge() {
  // 1 MB with every byte value ('\0' included): partial sends and buffer boundaries (N2, N3)
  std::string data(1 << 20, '\0');
  for(std::size_t i = 0 ; i < data.size() ; ++i) {
    data[i] = static_cast<char>(i * 7 % 256);
  }
  TcpServer server("127.0.0.1", 0);
  anch::ut::ThreadErrors errors;
  std::string received;
  std::thread srv = serve(server, errors, [&received](TcpStream& peer) {
    received.assign(std::istreambuf_iterator<char>(peer), std::istreambuf_iterator<char>()); // until shutdown
    peer << received.size() << std::endl;
  });
  TcpStream client("127.0.0.1", server.getAddress().getPort(), 1000); // small buffer
  client.getSocket().setReceiveTimeout(TIMEOUT);
  client.write(data.data(), static_cast<std::streamsize>(data.size()));
  client.shutdown(Direction::TRANSMISSION); // half-close: response can still be read
  std::size_t size = 0;
  client >> size;
  srv.join();
  errors.rethrow();
  anch::ut::assertThat(size == data.size(), "Server has read {} bytes", size);
  anch::ut::assertTrue(received == data, "Received data differ");
}

void
testTcpEof() {
  TcpServer server("127.0.0.1", 0);
  anch::ut::ThreadErrors errors;
  std::thread srv = serve(server, errors, [](TcpStream& peer) {
    peer << "last words";
  }); // destroyed: pending data are sent, then connection is closed
  TcpStream client("127.0.0.1", server.getAddress().getPort());
  client.getSocket().setReceiveTimeout(TIMEOUT);
  std::string word;
  client >> word >> word;
  anch::ut::assertThat(word == "words", "Last word is '{}'", word);
  client >> word;
  anch::ut::assertTrue(client.eof() && !client.bad(), "End of stream expected");
  srv.join();
  errors.rethrow();
}

void
testTcpIPv6() {
  std::unique_ptr<TcpServer> server;
  try {
    server = std::make_unique<TcpServer>("::1", 0);
  } catch(const IOException& e) {
    std::cout << "IPv6 loopback is not available, test is skipped: " << e.what() << std::endl;
    return;
  }
  Address address = server->getAddress();
  anch::ut::assertThat(address.getFamily() == AF_INET6 && address.toString() == "[::1]:" + std::to_string(address.getPort()),
		       "Address is {}", address.toString());
  anch::ut::ThreadErrors errors;
  std::thread srv = serve(*server, errors, [](TcpStream& peer) {
    anch::ut::assertThat(peer.getSocket().getPeerAddress().getHost() == "::1", "Peer is {}", peer.getSocket().getPeerAddress().toString());
    peer << "v6" << std::endl;
  });
  TcpStream client("::1", address.getPort());
  client.getSocket().setReceiveTimeout(TIMEOUT);
  std::string line;
  std::getline(client, line);
  srv.join();
  errors.rethrow();
  anch::ut::assertThat(line == "v6", "Response is '{}'", line);
}

void
testTcpRefused() {
  uint16_t port = 0;
  {
    TcpServer server("127.0.0.1", 0); // free port, closed at end of scope
    port = server.getAddress().getPort();
  }
  try {
    TcpStream client("127.0.0.1", port);
  } catch(const IOException& e) {
    anch::ut::assertThat(e.code().value() == ECONNREFUSED, "Unexpected error: {}", e.what());
    return;
  }
  anch::ut::fail("Connection should be refused");
}

void
testTcpReceiveTimeout() {
  TcpServer server("127.0.0.1", 0);
  TcpStream client("127.0.0.1", server.getAddress().getPort());
  Socket peer = server.acceptSocket(); // never writes
  client.getSocket().setReceiveTimeout(100ms);
  client.exceptions(std::ios::badbit);
  std::string line;
  try {
    std::getline(client, line);
  } catch(const IOException& e) {
    anch::ut::assertThat(e.code().value() == EAGAIN || e.code().value() == EWOULDBLOCK, "Unexpected error: {}", e.what());
    return;
  }
  anch::ut::fail("Timeout should raise IOException");
}

void
testSocketOwnership() {
  Socket sock(AF_INET, SocketType::TCP);
  int fd = sock.getDescriptor();
  Socket moved(std::move(sock));
  anch::ut::assertTrue(!sock.isOpen() && moved.getDescriptor() == fd, "Descriptor should be moved");
  moved = Socket();
  anch::ut::assertThat(::fcntl(fd, F_GETFD) == -1 && errno == EBADF, "Descriptor {} should be closed", fd);
}

void
testUdp() {
  UdpSocket server("127.0.0.1", 0);
  UdpSocket client("127.0.0.1", 0);
  server.getSocket().setReceiveTimeout(TIMEOUT);
  client.getSocket().setReceiveTimeout(TIMEOUT);
  std::string datagram("with\0zero", 9);
  client.sendTo(datagram, "127.0.0.1", server.getAddress().getPort());
  std::string received;
  Address from = server.receiveFrom(received);
  anch::ut::assertTrue(received == datagram, "Datagram should keep '\\0'");
  anch::ut::assertThat(from == client.getAddress(), "Sender is {} instead of {}", from.toString(), client.getAddress().toString());
  server.sendTo("pong", from);
  client.receiveFrom(received);
  anch::ut::assertThat(received == "pong", "Response is '{}'", received);
}

void
testUdpTruncated() {
  // N9: datagrams were silently truncated to 1024 bytes
  UdpSocket server("127.0.0.1", 0);
  UdpSocket client("127.0.0.1", 0);
  server.getSocket().setReceiveTimeout(TIMEOUT);
  client.sendTo(std::string(2000, 'x'), server.getAddress());
  std::string received;
  try {
    server.receiveFrom(received, 1024);
  } catch(const IOException& e) {
    anch::ut::assertThat(e.code().value() == EMSGSIZE, "Unexpected error: {}", e.what());
    return;
  }
  anch::ut::fail("Truncated datagram should raise IOException");
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH network unit tests")
    .description("Test AnCH sockets, TCP streams and UDP datagrams")
    .add("tcp-echo", testTcpEcho)
    .add("tcp-large", testTcpLarge)
    .add("tcp-eof", testTcpEof)
    .add("tcp-ipv6", testTcpIPv6)
    .add("tcp-refused", testTcpRefused)
    .add("tcp-receive-timeout", testTcpReceiveTimeout)
    .add("socket-ownership", testSocketOwnership)
    .add("udp", testUdp)
    .add("udp-truncated", testUdpTruncated)
    ;
}
