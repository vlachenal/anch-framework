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

#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>


using std::string;
using std::cout;
using std::cerr;
using std::endl;

using anch::device::Network;
using anch::device::DeviceException;


// Static members initialization +
Network* Network::_self = NULL;
// Static members initialization -


// Constructors +
/**
 * Network configuration private constructor
 *
 * @param ifaceName Network interface name
 *
 * @throw DeviceException Device initialization error
 */
Network::Network(const string& ifaceName) throw(DeviceException) :
  _ifaceName(ifaceName) {

  // Open hardware controller to retrieve network informations +
  struct ifreq request;
  memset(&request, 0x00, sizeof(request));
  strcpy(request.ifr_name, _ifaceName.c_str());
  request.ifr_flags = IFF_UP | IFF_BROADCAST | IFF_RUNNING | IFF_MULTICAST;
  // TODO IPv6 with PF_INET6
  int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
  int ret = ioctl(sock, SIOCGIFBRDADDR, &request);
  close(sock);
  // Open hardware controller to retrieve network informations -
  if(ret == 0) {
    _broadcastAddr = inet_ntoa(((sockaddr_in*)(&request.ifr_broadaddr))->sin_addr);
  } else {
    throw DeviceException("Error on retrieving interface");
  }

}
// Constructors -

// Destructor +
/**
 * Network configuration destructor
 */
Network::~Network() {
  // Nothing to do
}
// Destructor -

// Methods +
/**
 * Initialize network configuration
 *
 * @param ifaceName Network interface name
 *
 * @throw DeviceException Interface initialization error
 */
void
Network::initialize(const string& ifaceName) throw(DeviceException) {
  if(_self == NULL) {
    _self = new Network(ifaceName);
  } else if(_self->_ifaceName != ifaceName) {
    delete _self;
    _self = new Network(ifaceName);
  }
}
// Methods -
