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
#pragma once

namespace anch::device {

  inline const std::string& NetworkInterface::getName() const {
    return _name;
  }

  inline const std::string& NetworkInterface::getIpAddress() const {
    return _ipAddress;
  }

  inline const std::string& NetworkInterface::getMacAddress() const {
    return _macAddress;
  }

  inline const std::string& NetworkInterface::getBroadcastAddress() const {
    return _broadcastAddress;
  }

  inline const std::string& NetworkInterface::getNetmask() const {
    return _netmask;
  }

  inline int NetworkInterface::getMtu() const {
    return _mtu;
  }

  inline int NetworkInterface::getMetric() const {
    return _metric;
  }

  inline bool NetworkInterface::isLocalhost() const {
    return _localhost;
  }

}
