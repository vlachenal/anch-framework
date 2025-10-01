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

namespace anch::ini {

  /*!*
   * Parse error exception
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class ParserError: public std::exception {

  public:
    /*!*
     * \ref ParserError error code definition
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
      PARSING_ERROR

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
     * Forbids \ref ParserError default constructor
     */
    ParserError() = delete;

    /*!
     * \ref ParserError constructor
     *
     * \param msg the error message
     * \param code the error code (default to UNKNOWN)
     */
    ParserError(const std::string& msg, ParserError::ErrorCode code = ErrorCode::UNKNOWN);
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref ParserError destructor
     */
    virtual ~ParserError() noexcept;
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
    ParserError::ErrorCode getErrorCode() const noexcept;
    // Methods -

  };

}

#include "ini/impl/parserError.hpp"
