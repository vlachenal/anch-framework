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

#include <arpa/inet.h>


using anch::device::NetworkInterface;


// Constructors +
/**
 * {@link NetworkInterface} constructor.
 *
 * @param interface Data retrieved through <code>ioctl</code> POSIX call
 */
NetworkInterface::NetworkInterface(const struct ifreq& interface) {
  _name = interface.ifr_name;
  _ipAddress = inet_ntoa(((sockaddr_in *)&(interface.ifr_addr))->sin_addr);
  char buffer[17];
  sprintf(buffer,"%02x:%02x:%02x:%02x:%02x:%02x",
	  (unsigned char)interface.ifr_hwaddr.sa_data[0],
	  (unsigned char)interface.ifr_hwaddr.sa_data[1],
	  (unsigned char)interface.ifr_hwaddr.sa_data[2],
	  (unsigned char)interface.ifr_hwaddr.sa_data[3],
	  (unsigned char)interface.ifr_hwaddr.sa_data[4],
	  (unsigned char)interface.ifr_hwaddr.sa_data[5]);
  _macAddress = buffer;
  _broadcastAddress = inet_ntoa(((sockaddr_in*)&(interface.ifr_broadaddr))->sin_addr);
  _netmask = inet_ntoa(((sockaddr_in*)&(interface.ifr_netmask))->sin_addr);
  _mtu = interface.ifr_mtu;
  _metric = interface.ifr_metric;
}
// Constructors -


// Destructor +
/**
 * {@link NetworkInterface} destructor
 */
NetworkInterface::~NetworkInterface() {
  // Nothing to do
}
// Destructor -
