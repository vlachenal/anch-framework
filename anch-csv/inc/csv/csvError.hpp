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

#include "anch-csv-export.hpp"

#include <cstddef>
#include <stdexcept>
#include <string>

namespace anch::csv {

  /*!
   * CSV error code
   *
   * \since 0.3
   */
  enum class ErrorCode {
    /*! Invalid CSV: quote in an unquoted field, character after a closing quote, unterminated quoted field, CR
        without LF */
    INVALID_FORMAT,
    /*! Record larger than \c Options::maxRecordSize or with more than \c Options::maxColumns columns */
    TOO_LARGE,
    /*! Record without the number of columns of the first record (\c Options::sameColumnCount ) */
    COLUMN_COUNT,
    /*! Unknown column (objects mapping) */
    UNKNOWN_COLUMN,
    /*! Missing required column (objects mapping) */
    MISSING_COLUMN,
    /*! Value conversion error (objects mapping) */
    CONVERSION,
    /*! Field constraint violation (objects mapping, anch-val constraints) */
    CONSTRAINT_VIOLATION
  };

  /*!
   * CSV error, with the position of the error: line (physical line, from \c 1 ) and column (field, from \c 1 ,
   * \c 0 when it is not known)
   *
   * \author Vincent Lachenal
   *
   * \since 0.3
   */
  class ANCH_CSV_EXPORT CsvError: public std::runtime_error {

    // Attributes +
  private:
    /*! Error code */
    anch::csv::ErrorCode _code;

    /*! Line */
    std::size_t _line;

    /*! Column */
    std::size_t _column;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref CsvError constructor: the message is prefixed by the position (<tt>line 3, column 2: ...</tt>)
     *
     * \param code the error code
     * \param message the error message
     * \param line the line
     * \param column the column
     */
    CsvError(anch::csv::ErrorCode code, const std::string& message, std::size_t line, std::size_t column);
    // Constructors -

    // Accessors +
  public:
    /*!
     * Error code getter
     *
     * \return the error code
     */
    anch::csv::ErrorCode getErrorCode() const noexcept;

    /*!
     * Line getter
     *
     * \return the line, from \c 1
     */
    std::size_t getLine() const noexcept;

    /*!
     * Column getter
     *
     * \return the column, from \c 1 , \c 0 when it is not known
     */
    std::size_t getColumn() const noexcept;
    // Accessors -

  };

}
