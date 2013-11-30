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
#include "file/fileException.hpp"

#include <string.h>


using std::string;

using anch::file::FileException;


// Constructors +
/*!
 * \ref FileException constructor
 *
 * \param message The error message
 */
FileException::FileException(const string& message): _message(message) {
  _message.shrink_to_fit();
}

/*!
 * \ref FileException constructor
 *
 * \param message The error message
 * \param errnum System error number
 */
FileException::FileException(const std::string& message, int errnum) {
  char buffer[1024];
  ::strerror_r(errnum,buffer,1024);
  _message = message + ": " + buffer;
  _message.shrink_to_fit();
}
// Constructors -

// Destructor +
/*!
 * \ref FileException destructor
 */
FileException::~FileException() noexcept {
  // Nothing to do
}
// Destructor -


// Methods +
/*!
 * Display message error
 *
 * \return The message error
 */
const char*
FileException::what() const noexcept {
  return _message.data();
}
// Methods -
