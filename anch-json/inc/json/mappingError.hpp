/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
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
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

#include <istream>
#include <exception>
#include <optional>
#include <string>
#include <string_view>
#include <cstddef>

#include "json/readerContext.hpp"


namespace anch::json {

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

    /*! Required field is missing
     *
     * \since 0.2
     */
    MISSING_FIELD,

    /*! Value does not respect a field constraint (\c null value in a required field which can not be \c null , ...)
     *
     * \since 0.2
     */
    CONSTRAINT_VIOLATION,

    /*! Input stream is larger than \c max_payload option
     *
     * \since 0.2
     */
    PAYLOAD_TOO_LARGE,

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

    /*! Error message (path included) */
    std::string _msg;

    /*! Error message without path */
    std::string _detail;

    /*! Path of the value in error (JSON Pointer) */
    std::string _path;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref MappingError constructor
     *
     * \param code the error code
     * \param context the JSON context parser
     */
    MappingError(ErrorCode code, const anch::json::ReaderContext& context);

    /*!
     * \ref MappingError constructor with detailed message.\n
     * The message replaces the generic message of the error code ; the input around the error is still added.
     *
     * \param code the error code
     * \param context the JSON context parser
     * \param message the detailed error message
     */
    MappingError(ErrorCode code, const anch::json::ReaderContext& context, std::string_view message);

    /*!
     * \ref MappingError constructor without input (serialization error)
     *
     * \param code the error code
     * \param message the error message
     *
     * \since 0.2
     */
    MappingError(ErrorCode code, std::string_view message);
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

    /*!
     * Add object field at the beginning of the path. It is used by mappers when error is propagated from a field
     * value.
     *
     * \param field the field name (escaped according to JSON Pointer rules: \c ~ as \c ~0 and \c / as \c ~1 )
     *
     * \since 0.2
     */
    void prependField(std::string_view field);

    /*!
     * Add array index at the beginning of the path. It is used by mappers when error is propagated from an array
     * item.
     *
     * \param index the item index
     *
     * \since 0.2
     */
    void prependIndex(std::size_t index);

  private:
    /*!
     * Compute error message from path and detail
     */
    void computeMessage();
    // Methods -

    // Accessors +
  public:
    /*!
     * Error code getter
     *
     * \return the error code
     */
    anch::json::ErrorCode getErrorCode() const;

    /*!
     * Path of the value in error getter.\n
     * The path is a JSON Pointer (RFC 6901) from the deserialized root value: \c /pets/3/name for the \c name
     * field of the fourth item of the \c pets array. It is empty when the error is not in a field or array item
     * value (root value, field name or separator).\n
     * When path is not empty, \c what() message starts with the path followed by \c ": " .
     *
     * \return the path
     *
     * \since 0.2
     */
    const std::string& getPath() const;
    // Accessors -

  };

}  // anch::json

#include "json/impl/mappingError.hpp"
