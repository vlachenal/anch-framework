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

#include "rest/httpException.hpp"
#include "rest/response.hpp"

namespace anch::rest {

  /*!
   * Error response (status >= 400) received by \ref Client .
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  class HttpError: public anch::rest::HttpException {

    // Attributes +
  private:
    /*! The error response */
    anch::rest::Response _response;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref HttpError constructor
     *
     * \param response the error response
     */
    explicit HttpError(anch::rest::Response response);
    // Constructors -

    // Accessors +
  public:
    /*!
     * Response getter
     *
     * \return the error response (headers and body)
     */
    const anch::rest::Response& getResponse() const noexcept;
    // Accessors -

  };

}
