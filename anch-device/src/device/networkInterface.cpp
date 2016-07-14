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
#include "device/networkInterface.hpp"

#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


using anch::device::NetworkInterface;
using anch::device::DeviceException;


// Constructors +
NetworkInterface::NetworkInterface(const struct ifreq& interface, bool isLocalhost)
  throw(DeviceException) {
  _name = interface.ifr_name;
  _localhost = isLocalhost;

  // Open socket +
  int sock = ::socket(AF_INET, SOCK_DGRAM, 0);
  if(sock < 0) {
    throw DeviceException("Error while opening socket",sock);
  }
  // Open socket -

  int ret = ::ioctl(sock, SIOCGIFFLAGS, &interface);
  if(ret < 0) {
    ::close(sock);
    throw DeviceException("Error while retrieving flags for interface " + _name,ret);
  }
  // IP address +
  ret = ::ioctl(sock, SIOCGIFADDR, &interface);
  if(ret < 0) {
    ::close(sock);
    throw DeviceException("Error while retrieving IP address for interface " + _name,ret);
  }
  _ipAddress = inet_ntoa(((sockaddr_in *)&(interface.ifr_addr))->sin_addr);
  // IP address -
  // MTU +
  ret = ::ioctl(sock, SIOCGIFMTU, &interface);
  if(ret < 0) {
    ::close(sock);
    throw DeviceException("Error while retrieving MTU for interface " + _name,ret);
  }
  _mtu = interface.ifr_mtu;
  // MTU -
  // MAC address +
  ret = ::ioctl(sock, SIOCGIFHWADDR, &interface);
  if(ret < 0) {
    ::close(sock);
    throw DeviceException("Error while retrieving hardware address for interface " + _name,ret);
  }
  char buffer[17];
  sprintf(buffer,"%02x:%02x:%02x:%02x:%02x:%02x",
	  (unsigned char)interface.ifr_hwaddr.sa_data[0],
	  (unsigned char)interface.ifr_hwaddr.sa_data[1],
	  (unsigned char)interface.ifr_hwaddr.sa_data[2],
	  (unsigned char)interface.ifr_hwaddr.sa_data[3],
	  (unsigned char)interface.ifr_hwaddr.sa_data[4],
	  (unsigned char)interface.ifr_hwaddr.sa_data[5]);
  _macAddress = buffer;
  // MAC address -
  // Broadcast address +
  ret = ::ioctl(sock, SIOCGIFBRDADDR, &interface);
  if(ret < 0) {
    ::close(sock);
    throw DeviceException("Error while retrieving broadcast address for interface " + _name,ret);
  }
  _broadcastAddress = inet_ntoa(((sockaddr_in*)&(interface.ifr_broadaddr))->sin_addr);
  // Broadcast address -
  // Netmask +
  ret = ::ioctl(sock, SIOCGIFNETMASK, &interface);
  if(ret < 0) {
    ::close(sock);
    throw DeviceException("Error while retrieving netmask for interface " + _name,ret);
  }
  _netmask = inet_ntoa(((sockaddr_in*)&(interface.ifr_netmask))->sin_addr);
  // Netmask -
  _metric = interface.ifr_metric;


  // Release resources +
  ::close(sock);
  // Release resources -
}
// Constructors -


// Destructor +
NetworkInterface::~NetworkInterface() {
  // Nothing to do
}
// Destructor -
