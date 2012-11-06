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

#ifdef ANCH_POSIX
#define LOCALHOST "lo"
#else
#define LOCALHOST "lo"
#endif

using std::string;
using std::map;
using std::pair;
using std::mutex;

using anch::device::Network;
using anch::device::NetworkInterface;
using anch::device::DeviceException;


// Static members initialization +
mutex Network::MUTEX;
map<string,NetworkInterface>* Network::_interfaces = NULL;
// Static members initialization -


// Constructors +
/**
 * Network configuration private constructor
 */
Network::Network() {
  // Nothing to do
}
// Constructors -

// Destructor +
/**
 * Network configuration destructor
 */
Network::~Network() {
  if(_interfaces != NULL) {
    delete _interfaces;
  }
}
// Destructor -

// Methods +
/**
 * Retrieve network interface by its name.
 *
 * @param ifName The interface name
 *
 * @return The interface if found, <code>NULL</code> otherwise
 *
 * @throw DeviceException Network interfaces error
 */
const NetworkInterface* const
Network::getInterface(const string& ifName) throw(DeviceException) {
  const NetworkInterface* interface = NULL;
  MUTEX.lock();
  if(_interfaces == NULL) {
    load();
  }
  map<string,NetworkInterface>::const_iterator iter = _interfaces->find(ifName);
  if(iter != _interfaces->cend()) {
    interface = &(iter->second);
  }
  MUTEX.unlock();
  return interface;
}

/**
 * Retrieve all network interfaces.
 *
 * @return The network interfaces
 *
 * @throw DeviceException Network interfaces error
 */
const map<string,NetworkInterface>&
Network::getInterfaces() throw(DeviceException) {
  MUTEX.lock();
  if(_interfaces == NULL) {
    load();
  }
  MUTEX.unlock();
  return *_interfaces;
}

/**
 * Reload network interfaces
 *
 * @throw DeviceException Network interfaces error
 */
void
Network::reload() throw(DeviceException) {
  MUTEX.lock();
  if(_interfaces != NULL) {
    _interfaces->clear();
  }
  load();
  MUTEX.unlock();
}


/**
 * Load network interfaces
 *
 * @throw DeviceException Network interfaces error
 */
void
Network::load() throw(DeviceException) {
  // Initialize interfaces container if needed +
  if(_interfaces == NULL) {
    _interfaces = new map<string,NetworkInterface>();
  }
  // Initialize interfaces container if needed -

  // Open socket +
  int sock = ::socket(AF_INET, SOCK_DGRAM, 0);
  if(sock < 0) {
    throw DeviceException("Error while opening socket",sock);
  }
  // Open socket -

  // Request for interfaces configuration +
  char buffer[1024];
  struct ifconf request;
  request.ifc_len = sizeof(buffer);
  request.ifc_buf = buffer;
  int ret = ::ioctl(sock, SIOCGIFCONF, &request);
  if(ret < 0) {
    throw DeviceException("Error while retrieving interfaces configuration",ret);
  }
  // Request for interfaces configuration -

  // Retrieve all network interfaces configuration +
  struct ifreq* req = request.ifc_req;
  int nbResult = request.ifc_len / sizeof(struct ifreq);
  for(int i = 0 ; i < nbResult ; i++) {
    struct ifreq* interface = &req[i];
    string ifName = interface->ifr_name;
    bool isLoopback = (ifName == LOCALHOST);
    _interfaces->insert(pair<string,NetworkInterface>(ifName, NetworkInterface(*interface,isLoopback)));
  }
  // Retrieve all network interfaces configuration -

  // Release resources +
  ::close(sock);
  // Release resources -
}
// Methods -
