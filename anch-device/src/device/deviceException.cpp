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
#include "device/deviceException.hpp"

#include <errno.h>
#include <netdb.h>


using std::string;

using anch::device::DeviceException;


// Constructors +
DeviceException::DeviceException(const string& message) noexcept :
  _message(message) {
  // Do nothing
}

DeviceException::DeviceException(const string& message, int errorCode) noexcept {
  _message = message + ": " + ::gai_strerror(errorCode);
}
// Constructors -

// Destructor +
DeviceException::~DeviceException() noexcept {
  // Do nothing
}
// Destructor -

// Methods +
const char*
DeviceException::what() const noexcept {
  return _message.data();
}
// Methods -
