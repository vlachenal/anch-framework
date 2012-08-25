/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standart
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
#include <netdb.h>


using std::string;
using std::exception;

using anch::network::IOException;


// Constructors +
/**
 * {@link IOException} constructor
 *
 * @param message The error message
 */
IOException::IOException(const string& message): _message(message) {
  _message = message + ": " + sys_errlist[errno];
}

/**
 * {@link IOException} constructor
 *
 * @param message The error message
 * @param errorCode The error code (from <code>getinfoaddr</code>)
 */
IOException::IOException(const string& message, int errorCode) {
  _message = message + ": " + gai_strerror(errorCode);
}
// Constructors -

// Destructor +
/**
 * {@link IOException} destructor
 */
IOException::~IOException() throw() {
  // Nothing to do
}
// Destructor -


/**
 * Return the error message
 *
 * @return The error message
 */
const char*
IOException::what() const throw() {
  return _message.data();
}
