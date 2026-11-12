/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
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
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

#include <cerrno>
#include <string>
#include <system_error>

namespace anch::network {

  /*!
   * \c getaddrinfo error category (messages from \c gai_strerror )
   *
   * \return the category
   *
   * \since 0.2
   */
  const std::error_category& addrinfoCategory() noexcept;

  /*!
   * \brief Network input/output error
   *
   * The error code is available with \c code() : \c std::generic_category for system errors (\c errno values),
   * \ref addrinfoCategory for address resolution errors.
   *
   * \author Vincent Lachenal
   */
  class IOException: public std::system_error {

  public:
    // Constructors +
    /*!
     * \ref IOException constructor
     *
     * \param message the error message
     * \param errnum the \c errno value (default to current \c errno )
     */
    explicit IOException(const std::string& message, int errnum = errno);

    /*!
     * \ref IOException constructor
     *
     * \param message the error message
     * \param code the error code
     *
     * \since 0.2
     */
    IOException(const std::string& message, const std::error_code& code);
    // Constructors -

    // Destructor +
    /*!
     * \ref IOException destructor
     */
    virtual ~IOException() noexcept;
    // Destructor -

  };

}  // anch::network
