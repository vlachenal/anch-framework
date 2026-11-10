/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
  Copyright (C) 2012 Vincent Lachenal

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
  along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "device/network.hpp"

#include <mutex>
#include <set>

#include <ifaddrs.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netpacket/packet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

using anch::device::Network;
using anch::device::NetworkInterface;
using anch::device::DeviceException;

/*! Network interfaces indexed by their names */
using Interfaces = std::map<std::string,NetworkInterface>;

namespace {

  /*! Current network interfaces mutex */
  std::mutex MUTEX;

  /*! Current network interfaces (immutable snapshot replaced on reload) */
  std::shared_ptr<const Interfaces> INTERFACES;

  /*!
   * Format IPv4 / IPv6 address (with scope for IPv6 link-local addresses)
   *
   * \param address the address (can be \c NULL )
   *
   * \return the address, empty if not available
   */
  std::string
  toString(const struct sockaddr* address) {
    if(address == NULL) {
      return std::string();
    }
    socklen_t length;
    switch(address->sa_family) {
    case AF_INET:
      length = sizeof(struct sockaddr_in);
      break;
    case AF_INET6:
      length = sizeof(struct sockaddr_in6);
      break;
    default:
      return std::string();
    }
    char host[NI_MAXHOST];
    if(::getnameinfo(address, length, host, sizeof(host), NULL, 0, NI_NUMERICHOST) != 0) {
      return std::string();
    }
    return std::string(host);
  }

  /*!
   * Format hardware address
   *
   * \param link the \c AF_PACKET address (\c glibc stores up to 24 bytes in \c sll_addr )
   *
   * \return the hardware address (hexadecimal bytes separated by \c : ), empty if none
   */
  std::string
  toHardwareAddress(const struct sockaddr_ll* link) {
    static const char HEX[] = "0123456789abcdef";
    const unsigned char* bytes = link->sll_addr;
    std::string mac;
    for(std::size_t i = 0 ; i < link->sll_halen && i < 24 ; ++i) {
      if(i > 0) {
	mac += ':';
      }
      mac += HEX[bytes[i] >> 4];
      mac += HEX[bytes[i] & 0x0F];
    }
    return mac;
  }

  /*!
   * Retrieve interface information with \c ioctl
   *
   * \param sock the socket
   * \param name the interface name
   * \param request the \c ioctl request
   * \param result the result
   *
   * \return \c true on success, \c false otherwise
   */
  bool
  query(int sock, const std::string& name, unsigned long request, struct ifreq& result) {
    result = {};
    name.copy(result.ifr_name, IFNAMSIZ - 1);
    return ::ioctl(sock, request, &result) == 0;
  }

}

// Methods +
std::optional<NetworkInterface>
Network::getInterface(const std::string& ifName) {
  std::shared_ptr<const Interfaces> interfaces = snapshot();
  auto iter = interfaces->find(ifName);
  if(iter == interfaces->cend()) {
    return std::nullopt;
  }
  return iter->second;
}

Interfaces
Network::getInterfaces() {
  return *snapshot();
}

void
Network::reload() {
  auto interfaces = std::make_shared<const Interfaces>(load());
  std::lock_guard<std::mutex> lock(MUTEX);
  INTERFACES = std::move(interfaces);
}

std::shared_ptr<const Interfaces>
Network::snapshot() {
  std::lock_guard<std::mutex> lock(MUTEX);
  if(!INTERFACES) {
    INTERFACES = std::make_shared<const Interfaces>(load());
  }
  return INTERFACES;
}

Interfaces
Network::load() {
  // Addresses +
  struct ifaddrs* addresses = NULL;
  if(::getifaddrs(&addresses) != 0) {
    throw DeviceException("Error while listing network interfaces");
  }
  Interfaces interfaces;
  std::set<std::string> broadcast;
  try {
    for(const struct ifaddrs* iter = addresses ; iter != NULL ; iter = iter->ifa_next) {
      auto found = interfaces.find(iter->ifa_name);
      if(found == interfaces.end()) {
	found = interfaces.emplace(iter->ifa_name, NetworkInterface(iter->ifa_name)).first;
      }
      NetworkInterface& interface = found->second;
      interface._up = (iter->ifa_flags & IFF_UP) != 0;
      interface._localhost = (iter->ifa_flags & IFF_LOOPBACK) != 0;
      if(iter->ifa_addr == NULL) {
	continue;
      }
      switch(iter->ifa_addr->sa_family) {
      case AF_PACKET:
	interface._macAddress = toHardwareAddress(reinterpret_cast<const struct sockaddr_ll*>(iter->ifa_addr));
	break;
      case AF_INET:
	if(interface._ipAddress.empty()) {
	  interface._ipAddress = toString(iter->ifa_addr);
	  interface._netmask = toString(iter->ifa_netmask);
	  // ifa_broadaddr is the address itself when no broadcast address is set: retrieved with ioctl
	  if((iter->ifa_flags & IFF_BROADCAST) != 0) {
	    broadcast.insert(interface._name);
	  }
	}
	break;
      case AF_INET6:
	interface._ipv6Addresses.push_back(toString(iter->ifa_addr));
	break;
      default:
	break;
      }
    }
  } catch(...) {
    ::freeifaddrs(addresses);
    throw;
  }
  ::freeifaddrs(addresses);
  // Addresses -

  // MTU, metric and broadcast address (optional: the interface may have been removed since) +
  int sock = ::socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
  if(sock < 0) {
    throw DeviceException("Error while opening socket");
  }
  for(auto& [name, interface] : interfaces) {
    struct ifreq result;
    if(query(sock, name, SIOCGIFMTU, result)) {
      interface._mtu = result.ifr_mtu;
    }
    if(query(sock, name, SIOCGIFMETRIC, result)) {
      interface._metric = result.ifr_metric;
    }
    if(broadcast.contains(name) && query(sock, name, SIOCGIFBRDADDR, result)) {
      interface._broadcastAddress = toString(&result.ifr_broadaddr);
      if(interface._broadcastAddress == "0.0.0.0") {
	interface._broadcastAddress.clear();
      }
    }
  }
  ::close(sock);
  // MTU, metric and broadcast address -

  return interfaces;
}
// Methods -
