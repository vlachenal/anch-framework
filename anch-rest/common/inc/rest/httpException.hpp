/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2026 Vincent Lachenal

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

#include <stdexcept>
#include <string>

#include "rest/status.hpp"

namespace anch::rest {

  /*!
   * HTTP error.\n
   * Thrown by an endpoint, the response has the exception status and its message as
   * \c text/plain body (when it is not empty).
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  class HttpException: public std::runtime_error {

    // Attributes +
  private:
    /*! Status */
    anch::rest::Status _status;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref HttpException constructor
     *
     * \param status the status
     * \param message the message
     */
    HttpException(anch::rest::Status status, const std::string& message = std::string());
    // Constructors -

    // Accessors +
  public:
    /*!
     * Status getter
     *
     * \return the status
     */
    anch::rest::Status getStatus() const noexcept;
    // Accessors -

  };

}
