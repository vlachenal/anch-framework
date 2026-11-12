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

#include <optional>
#include <string_view>

namespace anch::rest {

  /*!
   * HTTP request methods
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  enum class Method {
    /*! GET method */
    GET,
    /*! HEAD method */
    HEAD,
    /*! POST method */
    POST,
    /*! PUT method */
    PUT,
    /*! DELETE method */
    DELETE,
    /*! CONNECT method */
    CONNECT,
    /*! OPTIONS method */
    OPTIONS,
    /*! TRACE method */
    TRACE,
    /*! PATCH method */
    PATCH
  };

  /*!
   * Get HTTP method name
   *
   * \param method the method
   *
   * \return the method name (\c "GET" , ...)
   */
  std::string_view toString(anch::rest::Method method) noexcept;

  /*!
   * Parse HTTP method name (case sensitive)
   *
   * \param name the method name
   *
   * \return the method, empty when the method is unknown
   */
  std::optional<anch::rest::Method> parseMethod(std::string_view name) noexcept;

}
