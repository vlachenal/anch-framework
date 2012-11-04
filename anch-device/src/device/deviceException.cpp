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


using std::string;

using anch::device::DeviceException;


// Constructors +
/**
 * {@link DeviceException} constructor
 *
 * @param message The error message
 */
DeviceException::DeviceException(const string message) throw() :
  _message(message) {
  // Do nothing
}
// Constructors -

// Destructor +
/**
 * {@link DeviceException} destructor
 */
DeviceException::~DeviceException() throw() {
  // Do nothing
}
// Destructor -

// Methods +
/**
 * Return the error message
 *
 * @return The error message
 */
const char*
DeviceException::what() const throw() {
  return _message.data();
}
// Methods -
