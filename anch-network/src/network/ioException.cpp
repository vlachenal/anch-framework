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
#include "network/ioException.hpp"

#include <errno.h>
#include <string.h>
#include <netdb.h>


using std::exception;

using anch::network::IOException;


// Constructors +
IOException::IOException(const std::string& message): _message(message) {
  _message = message + ": " + strerror(errno);
}

IOException::IOException(const std::string& message, int errorCode) {
  _message = message + ": " + gai_strerror(errorCode);
}
// Constructors -

// Destructor +
IOException::~IOException() noexcept {
  // Nothing to do
}
// Destructor -


const char*
IOException::what() const noexcept {
  return _message.data();
}
