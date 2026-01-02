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

#include <exception>
#include <string>

namespace anch::conf {

  /*!*
   * Parse error exception
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class ConfError: public std::exception {

  public:
    /*!*
     * \ref ConfError error code definition
     *
     * \author Vincent Lachenal
     *
     * \since 0.1
     */
    enum class ErrorCode {

      /*! Unknown error */
      UNKNOWN = 0,

      /*! File is not readable */
      NOT_READABLE,

      /*! Parsing error */
      PARSING_ERROR,

      /*! Configuration not found */
      CONF_NOT_FOUND,

      /*! Resolver not found */
      RES_NOT_FOUND,

      /*! Resolver is a protected resolver */
      RES_PROTECTED,

      /*! Parser not found */
      PARSER_NOT_FOUND,

      /*! Parser is a protected resolver */
      PARSER_PROTECTED,

      /*! Parser is not initialized */
      NOT_LOADED

    };

    // Attributes +
  private:
    /*! Error code */
    ErrorCode _code;

    /*! Error message */
    std::string _msg;
    // Attributes -

    // Constructors +
  public:
    /*!
     * Forbids \ref ConfError default constructor
     */
    ConfError() = delete;

    /*!
     * \ref ConfError constructor
     *
     * \param msg the error message
     * \param code the error code (default to UNKNOWN)
     */
    ConfError(const std::string& msg, ConfError::ErrorCode code = ErrorCode::UNKNOWN);
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref ConfError destructor
     */
    virtual ~ConfError() noexcept;
    // Destructor -

    // Methods +
  public:
    /*!
     * Get error message
     *
     * \return the error message
     */
    virtual const char* what() const noexcept;

    /*!
     * Error code getter
     *
     * \return the error code
     */
    ConfError::ErrorCode getErrorCode() const noexcept;
    // Methods -

  };

}

#include "conf/impl/confError.hpp"
