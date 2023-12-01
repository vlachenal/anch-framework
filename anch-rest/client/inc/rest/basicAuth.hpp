/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
  Copyright (C) 2020 Vincent Lachenal

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

#include <string>

#include "rest/auth.hpp"

namespace anch::rest {

  /*!
   * \brief Basic authentication
   *
   * HTTP basic authentication scheme implementation. This is not safe: base64(login:password)
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class BasicAuthentication: public anch::rest::Authentication {

    // Attributes +
  private:
    /*! The header value */
    std::string _auth;
    // Attributes -

    // Constructors +
  public:
    /*!
     * Forbid \ref BasicAuthentication default constructor
     */
    BasicAuthentication() = delete;

    /*!
     * \ref BasicAuthentication constructor
     *
     * \param login the login
     * \param password the password
     */
    BasicAuthentication(const std::string& login, const std::string& password);
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref BasicAuthentication destructor
     */
    virtual ~BasicAuthentication();
    // Destructor -

    // Methods +
  public:
    /*!
     * Fill headers with Authorization header.\n
     * Headers will fill with \c "Basic base64(login:password)"
     *
     * \param headers the request header to fill
     * \param header the header name (default to \c Authorization )
     */
    virtual void setAuthentication(anch::rest::Headers& headers, const std::string& header);
    // Methods -

  };

}  // anch::rest
