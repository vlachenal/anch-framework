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

#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <ostream>
#include <span>
#include <string_view>
#include <type_traits>

#include "csv/options.hpp"

namespace anch::csv::detail {

  /*!
   * Text of a field written by \ref anch::csv::Writer::writeRow : strings as is, \c bool as \c true / \c false ,
   * numbers with \c std::to_chars (independent of the locale)
   */
  class Text {
  private:
    /*! String value */
    std::string_view _view;

    /*! Number formatting buffer */
    char _buffer[64];

    /*! Number length, \c 0 for strings */
    std::size_t _length;

  public:
    /*!
     * Text of a string
     *
     * \param value the value
     */
    template<typename V>
    requires std::convertible_to<const V&, std::string_view>
    Text(const V& value);

    /*!
     * Text of a character
     *
     * \param value the value
     */
    Text(char value);

    /*!
     * Text of a boolean
     *
     * \param value the value
     */
    Text(bool value);

    /*!
     * Text of a number
     *
     * \param value the value
     */
    template<typename V>
    requires (std::is_arithmetic_v<V> && !std::same_as<V, bool> && !std::same_as<V, char>)
    Text(V value);

    /*! Forbids \ref Text copy constructor (view on its own buffer) */
    Text(const Text&) = delete;

    /*!
     * Get text
     *
     * \return the text
     */
    std::string_view value() const noexcept;
  };

}

namespace anch::csv {

  /*!
   * CSV records writer (RFC 4180): fields which contain the separator, the quote, CR or LF are quoted (quotes are
   * doubled), as a first field which begins with an UTF-8 BOM. Records end with \c CRLF (or \c LF , \c Options::crlf ). The header is written as a record
   * ( \c Options::header is not used by the writer) ; an UTF-8 BOM is written by the constructor with
   * \c Options::bom .
   * \snippet{trimleft} anch-csv/examples/csv.cpp writer
   *
   * \author Vincent Lachenal
   *
   * \since 0.3
   */
  class ANCH_CSV_EXPORT Writer {

    // Attributes +
  private:
    /*! Output stream */
    std::ostream& _output;

    /*! Options */
    anch::csv::Options _options;

    /*! Nothing has been written yet: a first field which begins with an UTF-8 BOM is quoted (a reader skips it) */
    bool _start;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref Writer constructor
     *
     * \param output the output stream
     * \param options the options
     *
     * \throw std::invalid_argument when separator and quote are equal, or one of them is CR or LF
     */
    Writer(std::ostream& output, const anch::csv::Options& options = anch::csv::Options());

    /*! Forbids \ref Writer copy constructor */
    Writer(const Writer&) = delete;
    // Constructors -

    // Methods +
  public:
    /*!
     * Write a record
     *
     * \param fields the fields
     */
    void write(std::span<const std::string_view> fields);

    /*!
     * Write a record
     *
     * \param fields the fields
     */
    void write(std::initializer_list<std::string_view> fields);

    /*!
     * Write a record of values: strings as is, \c bool as \c true / \c false , numbers with \c std::to_chars
     *
     * \param values the values
     */
    template<typename... V>
    void writeRow(const V&... values);
    // Methods -

  private:
    /*!
     * Write a field
     *
     * \param field the field
     * \param quote quote field even when it has no special character
     */
    void writeField(std::string_view field, bool quote);

  };

}

#include "csv/impl/writer.hpp"
