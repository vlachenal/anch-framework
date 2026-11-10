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

#include <cerrno>
#include <string>
#include <system_error>

namespace anch::device {

  /*!
   * \brief Device error
   *
   * The \c errno value of the failed system call is available with \c code() ( \c std::generic_category ) and its
   * description is appended to the message returned by \c what() .
   *
   * \author Vincent Lachenal
   */
  class DeviceException: public std::system_error {

  public:
    // Constructors +
    /*!
     * \ref DeviceException constructor
     *
     * \param message the error message
     * \param errnum the \c errno value (default to current \c errno )
     */
    explicit DeviceException(const std::string& message, int errnum = errno);
    // Constructors -

    // Destructor +
    /*!
     * \ref DeviceException destructor
     */
    virtual ~DeviceException() noexcept;
    // Destructor -

  };

}  // anch::device
