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
#include <istream>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "csv/csvError.hpp"
#include "csv/options.hpp"

namespace anch::csv {

  /*!
   * CSV records reader (RFC 4180): records are read one by one from a stream, without loading the whole input.\n
   * Fields are \c std::string_view valid until the next call to \ref next (the record buffer is reused: no
   * allocation per field). Quoted fields can contain the separator, doubled quotes and line endings. \c CRLF and
   * \c LF line endings are read (\c CR alone is refused), the last record can have no line ending and an UTF-8 BOM
   * is skipped. An empty field and an empty quoted field ( \c "" ) are distinguished with \ref quoted .\n
   * Errors raise \ref CsvError with the line and the column of the error.
   * \snippet{trimleft} anch-csv/examples/csv.cpp reader
   *
   * \author Vincent Lachenal
   *
   * \since 0.3
   */
  class ANCH_CSV_EXPORT Reader {

    // Attributes +
  private:
    /*! Input stream buffer */
    std::streambuf* _input;

    /*! Input stream (end of stream state) */
    std::istream& _stream;

    /*! Options */
    anch::csv::Options _options;

    /*! Bytes read before the stream buffer ones (skipped BOM lookahead) */
    std::string _pending;

    /*! Current record fields content */
    std::string _buffer;

    /*! End offset of each field in buffer */
    std::vector<std::size_t> _ends;

    /*! Quoted fields */
    std::vector<bool> _quoted;

    /*! Current record fields */
    std::vector<std::string_view> _fields;

    /*! Header */
    std::vector<std::string> _header;

    /*! Physical line of the next character, from 1 */
    std::size_t _line;

    /*! Physical line of current record */
    std::size_t _recordLine;

    /*! Size of current record in bytes */
    std::size_t _recordSize;

    /*! Expected number of columns, 0 when it is not known yet */
    std::size_t _columns;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref Reader constructor: the header is read when \c Options::header is set
     *
     * \param input the input stream
     * \param options the options
     *
     * \throw std::invalid_argument when separator and quote are equal, or one of them is CR or LF
     * \throw CsvError on invalid header
     */
    Reader(std::istream& input, const anch::csv::Options& options = anch::csv::Options());

    /*! Forbids \ref Reader copy constructor */
    Reader(const Reader&) = delete;
    // Constructors -

    // Methods +
  public:
    /*!
     * Read next record
     *
     * \return \c false at the end of the input
     *
     * \throw CsvError on invalid record
     */
    bool next();

    /*!
     * Get column index of a header name
     *
     * \param name the column name
     *
     * \return the first column with this name, empty when it is not in header
     */
    std::optional<std::size_t> column(std::string_view name) const noexcept;
    // Methods -

    // Accessors +
  public:
    /*!
     * Current record fields
     *
     * \return the fields, valid until the next call to \ref next
     */
    std::span<const std::string_view> fields() const noexcept;

    /*!
     * Current record number of fields
     *
     * \return the number of fields
     */
    std::size_t size() const noexcept;

    /*!
     * Current record field
     *
     * \param index the field index
     *
     * \return the field, valid until the next call to \ref next
     */
    std::string_view operator[](std::size_t index) const;

    /*!
     * Check if a field is quoted: an empty field is an absent value, an empty quoted field ( \c "" ) an empty
     * string
     *
     * \param index the field index
     *
     * \return \c true when field is quoted
     */
    bool quoted(std::size_t index) const;

    /*!
     * Physical line where the current record begins (records can have line endings in quoted fields)
     *
     * \return the line, from \c 1
     */
    std::size_t line() const noexcept;

    /*!
     * Header (empty without \c Options::header )
     *
     * \return the column names
     */
    const std::vector<std::string>& header() const noexcept;

    /*!
     * Options getter
     *
     * \return the options
     */
    const anch::csv::Options& options() const noexcept;
    // Accessors -

  private:
    /*!
     * Read a record in buffer
     *
     * \return \c false at the end of the input
     */
    bool read();

    /*!
     * Read a character
     *
     * \return the character, \c std::char_traits<char>::eof() at the end of the input
     */
    int get();

    /*!
     * Read a character when it is equal to an expected one
     *
     * \param expected the expected character
     *
     * \return \c true when the character has been read
     */
    bool accept(char expected);

    /*!
     * Raise error at the current position
     *
     * \param code the error code
     * \param message the message
     */
    [[noreturn]] void fail(anch::csv::ErrorCode code, const std::string& message) const;

  };

}
