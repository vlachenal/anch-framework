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

#include <atomic>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include "ut/assert.hpp"
#include "ut/unit.hpp"
#include "ut/threadErrors.hpp"

using anch::device::Network;
using anch::device::NetworkInterface;
using anch::device::DeviceException;


/*!
 * Interfaces names from \c if_nameindex
 */
std::set<std::string>
systemNames() {
  std::set<std::string> names;
  struct if_nameindex* indexes = ::if_nameindex();
  anch::ut::assertThat(indexes != NULL, "if_nameindex failed: {}", std::strerror(errno));
  for(struct if_nameindex* iter = indexes ; iter->if_index != 0 ; ++iter) {
    names.insert(iter->if_name);
  }
  ::if_freenameindex(indexes);
  return names;
}

/*!
 * Read first line of /sys/class/net/<name>/<file>
 */
std::string
readSys(const std::string& name, const std::string& file) {
  std::ifstream input("/sys/class/net/" + name + "/" + file);
  std::string line;
  std::getline(input, line);
  return line;
}

/*!
 * Retrieve IPv4 address with ioctl
 */
std::string
ioctlAddress(int sock, const std::string& name, unsigned long request) {
  struct ifreq req{};
  name.copy(req.ifr_name, IFNAMSIZ - 1);
  if(::ioctl(sock, request, &req) != 0) {
    return std::string();
  }
  char buffer[INET_ADDRSTRLEN];
  ::inet_ntop(AF_INET, &reinterpret_cast<const struct sockaddr_in*>(&req.ifr_addr)->sin_addr, buffer, sizeof(buffer));
  return std::string(buffer);
}

void
testAllInterfaces() {
  auto interfaces = Network::getInterfaces();
  std::set<std::string> names = systemNames();
  for(const std::string& name : names) {
    anch::ut::assertThat(interfaces.contains(name), "Interface {} should be listed", name);
  }
  for(const auto& [name, interface] : interfaces) {
    anch::ut::assertThat(name == interface.getName(), "Interface {} has name {}", name, interface.getName());
    // IPv4 address labels (eth0:1) are not interfaces
    anch::ut::assertThat(names.contains(name) || name.find(':') != std::string::npos, "Unknown interface {}", name);
    std::cout << name << ": IPv4 '" << interface.getIpAddress() << "', " << interface.getIpv6Addresses().size()
	      << " IPv6, MAC '" << interface.getMacAddress() << "', MTU " << interface.getMtu() << std::endl;
  }
}

void
testLoopback() {
  auto interfaces = Network::getInterfaces();
  const NetworkInterface* loopback = NULL;
  for(const auto& [name, interface] : interfaces) {
    if(interface.isLocalhost()) {
      loopback = &interface;
      break;
    }
  }
  anch::ut::assertThat(loopback != NULL, "Loopback interface not found");
  if(!loopback->isUp()) {
    return;  // Network namespace with loopback down
  }
  anch::ut::assertThat(loopback->getIpAddress() == "127.0.0.1", "Loopback address is '{}'", loopback->getIpAddress());
  anch::ut::assertThat(loopback->getNetmask() == "255.0.0.0", "Loopback netmask is '{}'", loopback->getNetmask());
  anch::ut::assertThat(loopback->getBroadcastAddress().empty(), "Loopback broadcast is '{}'", loopback->getBroadcastAddress());
  anch::ut::assertThat(loopback->getMtu() > 0, "Loopback MTU is {}", loopback->getMtu());
}

void
testIpv4Addresses() {
  int sock = ::socket(AF_INET, SOCK_DGRAM, 0);
  anch::ut::assertThat(sock >= 0, "socket failed: {}", std::strerror(errno));
  for(const auto& [name, interface] : Network::getInterfaces()) {
    std::string address = ioctlAddress(sock, name, SIOCGIFADDR);
    anch::ut::assertThat(interface.getIpAddress() == address, "{} IPv4 address: '{}' ; ioctl: '{}'", name, interface.getIpAddress(), address);
    if(address.empty()) {
      anch::ut::assertThat(interface.getNetmask().empty() && interface.getBroadcastAddress().empty(), "{} has no IPv4 address but has netmask or broadcast", name);
      continue;
    }
    std::string netmask = ioctlAddress(sock, name, SIOCGIFNETMASK);
    anch::ut::assertThat(interface.getNetmask() == netmask, "{} netmask: '{}' ; ioctl: '{}'", name, interface.getNetmask(), netmask);
    struct ifreq req{};
    name.copy(req.ifr_name, IFNAMSIZ - 1);
    anch::ut::assertThat(::ioctl(sock, SIOCGIFFLAGS, &req) == 0, "{} flags: {}", name, std::strerror(errno));
    // ioctl gives 0.0.0.0 when no broadcast address is set and destination address for point-to-point interfaces
    std::string broadcast = ioctlAddress(sock, name, SIOCGIFBRDADDR);
    if((req.ifr_flags & IFF_BROADCAST) == 0 || broadcast == "0.0.0.0") {
      broadcast.clear();
    }
    anch::ut::assertThat(interface.getBroadcastAddress() == broadcast, "{} broadcast: '{}' ; ioctl: '{}'", name, interface.getBroadcastAddress(), broadcast);
  }
  ::close(sock);
}

void
testIpv6Addresses() {
  // /proc/net/if_inet6: address (32 hexadecimal digits), index, prefix length, scope, flags, name
  std::map<std::string,std::multiset<std::string>> expected;
  std::ifstream input("/proc/net/if_inet6");
  std::string hex, index, prefix, scope, flags, name;
  while(input >> hex >> index >> prefix >> scope >> flags >> name) {
    struct in6_addr address;
    for(std::size_t i = 0 ; i < 16 ; ++i) {
      address.s6_addr[i] = static_cast<uint8_t>(std::stoul(hex.substr(i * 2, 2), NULL, 16));
    }
    char buffer[INET6_ADDRSTRLEN];
    ::inet_ntop(AF_INET6, &address, buffer, sizeof(buffer));
    std::string value(buffer);
    if(scope == "20") {  // Link-local
      value += "%" + name;
    }
    expected[name].insert(value);
  }
  for(const auto& [ifName, interface] : Network::getInterfaces()) {
    std::multiset<std::string> found(interface.getIpv6Addresses().begin(), interface.getIpv6Addresses().end());
    anch::ut::assertThat(found == expected[ifName], "{}: {} IPv6 addresses found ; {} expected", ifName, found.size(), expected[ifName].size());
    for(const std::string& address : found) {
      std::cout << ifName << ": " << address << std::endl;
    }
  }
}

void
testSysInformation() {
  for(const auto& [name, interface] : Network::getInterfaces()) {
    if(!systemNames().contains(name)) {
      continue;  // address label
    }
    std::string mac = readSys(name, "address");
    if(std::stoi(readSys(name, "addr_len")) == 0) {
      mac.clear();
    }
    anch::ut::assertThat(interface.getMacAddress() == mac, "{} MAC address: '{}' ; sysfs: '{}'", name, interface.getMacAddress(), mac);
    int mtu = std::stoi(readSys(name, "mtu"));
    anch::ut::assertThat(interface.getMtu() == mtu, "{} MTU: {} ; sysfs: {}", name, interface.getMtu(), mtu);
    unsigned long flags = std::stoul(readSys(name, "flags"), NULL, 16);
    anch::ut::assertThat(interface.isUp() == ((flags & IFF_UP) != 0), "{} up: {} ; flags: {:x}", name, interface.isUp(), flags);
    anch::ut::assertThat(interface.isLocalhost() == ((flags & IFF_LOOPBACK) != 0), "{} loopback: {} ; flags: {:x}", name, interface.isLocalhost(), flags);
    anch::ut::assertThat(interface.getMetric() == 0, "{} metric: {} (always 0 on Linux)", name, interface.getMetric());
  }
}

void
testGetInterface() {
  anch::ut::assertThat(!Network::getInterface("toto").has_value(), "Interface toto should not be found");
  for(const auto& [name, interface] : Network::getInterfaces()) {
    auto found = Network::getInterface(name);
    anch::ut::assertThat(found.has_value() && found->getName() == name, "Interface {} should be found", name);
    anch::ut::assertThat(found->getMacAddress() == interface.getMacAddress(), "Interface {}: different MAC address", name);
  }
}

void
testConcurrentReload() {
  auto expected = Network::getInterfaces();
  anch::ut::ThreadErrors errors;
  std::atomic<bool> stop(false);
  std::vector<std::thread> readers;
  for(int i = 0 ; i < 4 ; ++i) {
    readers.emplace_back(errors.wrap([&]() {
      while(!stop.load()) {
	auto interfaces = Network::getInterfaces();
	anch::ut::assertThat(interfaces.size() == expected.size(), "{} interfaces found ; {} expected", interfaces.size(), expected.size());
	for(const auto& [name, interface] : interfaces) {
	  anch::ut::assertThat(name == interface.getName(), "Interface {} has name {}", name, interface.getName());
	  auto found = Network::getInterface(name);
	  anch::ut::assertThat(found.has_value(), "Interface {} not found", name);
	}
      }
    }));
  }
  for(int i = 0 ; i < 500 && !errors.failed() ; ++i) {
    Network::reload();
  }
  stop.store(true);
  for(std::thread& reader : readers) {
    reader.join();
  }
  errors.rethrow();
}

void
testDeviceException() {
  DeviceException error("Error while listing network interfaces", ENODEV);
  anch::ut::assertThat(error.code() == std::errc::no_such_device, "Error code is {}", error.code().value());
  std::string message = error.what();
  anch::ut::assertThat(message == "Error while listing network interfaces: " + std::generic_category().message(ENODEV), "Message is '{}'", message);
  errno = EACCES;
  DeviceException current("Error");
  anch::ut::assertThat(current.code() == std::errc::permission_denied, "Default error code should be errno ; found {}", current.code().value());
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH device network unit tests")
    .description("Test AnCH network interfaces")
    .add("network-all-interfaces", testAllInterfaces)
    .add("network-loopback", testLoopback)
    .add("network-ipv4-addresses", testIpv4Addresses)
    .add("network-ipv6-addresses", testIpv6Addresses)
    .add("network-sys-information", testSysInformation)
    .add("network-get-interface", testGetInterface)
    .add("network-concurrent-reload", testConcurrentReload)
    .add("device-exception", testDeviceException)
    ;
}
