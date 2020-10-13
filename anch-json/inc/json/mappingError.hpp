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

#include <exception>


namespace anch {
  namespace json {

    /*!
     * JSON mapping error code
     *
     * \author Vincent Lachenal
     *
     * \since 0.1
     */
    enum class ErrorCode {
      /*! Invalid format */
      INVALID_FORMAT = 0,

      /*! Found unexpected field (and parsing options disallow it) */
      UNEXPECTED_FIELD,

      /*! Found too many space characters or charcters in field name (according to parsing option) */
      POTENTIAL_OVERFLOW,

      /*! Unexpected error code */
      UNKNOWN = 9999
    };

    /*!
     * \brief Mapping error
     *
     * Error which can be raised while parsing JSON input stream
     *
     * \author Vincent Lachenal
     *
     * \since 0.1
     */
    class MappingError: public std::exception {

      // Attributes +
    private:
      /*! Error code */
      ErrorCode _code;
      // Attributes -

      // Constructors +
    public:
      /*!
       * \ref MappingError default constructor
       */
      MappingError();
      // Constructors -

      // Destructor +
    public:
      /*!
       * \ref MappingError destructor
       */
      virtual ~MappingError();
      // Destructor -

      // Methods +
    public:
      /*!
       * Error message getter
       *
       * \return the error message
       */
      virtual const char* what() const noexcept override;
      // Methods -

      // Accessors +
      /*!
       * Error code getter
       *
       * \return the error code
       */
      anch::json::ErrorCode getErrorCode() const;
      // Accessors -

    };

  }  // json
}  // anch

#include "json/impl/mappingError.hpp"
