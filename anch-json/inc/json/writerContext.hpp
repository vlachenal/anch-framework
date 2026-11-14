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

#include <ostream>
#include <streambuf>
#include <string_view>
#include <string>
#include <functional>
#include <stdint.h>
#include <deque>

#include "json/mappingOptions.hpp"

namespace anch::json {

  /*!
   * \brief JSON writer context
   *
   * Context is used to store mapping options and serialization buffers
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class WriterContext {

    // Attributes +
  public:
    /*! The input stream to parse */
    std::ostream& output;

  private:
    /*! Mapping options */
    anch::json::MappingOptions _options;

    /*! First fields queue */
    std::deque<bool> _firsts;

    /*! Output stream buffer (written directly to avoid \c std::ostream sentry on each write) */
    std::streambuf* _sbuf;
    // Attributes -

    // Constructors +
  public:
    /*!
     * Forbids \c WriterContext default constructor
     */
    WriterContext() = delete;

    /*!
     * Forbids \c WriterContext copy constructor
     *
     * \param context the context not to copy
     */
    WriterContext(const WriterContext& context) = delete;

    /*!
     * Forbids \c WriterContext move constructor
     *
     * \param context the context not to move
     */
    WriterContext(WriterContext&& context) = delete;

    /*!
     * \c WriterContext constructor
     *
     * \param os the ouput stream to write in
     * \param options the mapping options to use
     */
    WriterContext(std::ostream& os, const anch::json::MappingOptions& options) noexcept;
    // Constructors -

    // Destructor +
  public:
    /*!
     * \c WriterContext destructor\n
     *
     * Delete buffers
     */
    virtual ~WriterContext() noexcept;
    // Destructor -

    // Methods +
  public:
    /*!
     * Mapping options getter
     *
     * \return the mapping options
     *
     * \since 0.2
     */
    const anch::json::MappingOptions& options() const noexcept;

    /*!
     * Write \c null value or not according to options
     *
     * \param field the field name
     *
     * \return \c true when \c null value has been serialize, \c false otherwise
     */
    bool writeNull(const std::string& field);

    /*!
     * Write empty collection according to options.\n
     * When serialize, it will be serialized as '"<field>":[]'
     *
     * \param field the field name
     */
    bool writeEmptyArray(const std::string& field);

    /*!
     * Write empty object (map) according to options.\n
     * When serialize, it will be serialized as '"<field>":{}'
     *
     * \param field the field name
     */
    bool writeEmptyObject(const std::string& field);

    /*!
     * Write field and add ':' into output stream.\n
     * Call \c next() to add ',' or not before field
     *
     * \param field the field
     */
    void writeField(const std::string& field);

    /*!
     * Write field without escaping it and add ':' into output stream.\n
     * Field has to be already escaped (it can contain formatting such as console colors).\n
     * Call \c next() to add ',' or not before field
     *
     * \param field the field
     */
    void writeRawField(std::string_view field);

    /*!
     * Write begin array ('[') and push first field in context
     */
    void beginArray();

    /*!
     * Write end array (']') and pop first field from context
     */
    void endArray();

    /*!
     * Write begin array ('{') and push first field in context
     */
    void beginObject();

    /*!
     * Write end array ('}') and pop first field from context
     */
    void endObject();

    /*!
     * Write field/value delimiter (',') according to context (first field or not)
     */
    void next();

    /*!
     * Write a character in output stream buffer.\n
     * Output stream \c badbit is set when character can not be written.
     *
     * \param c the character to write
     */
    void write(char c);

    /*!
     * Write characters in output stream buffer.\n
     * Output stream \c badbit is set when characters can not be written.
     *
     * \param str the characters to write
     */
    void write(std::string_view str);

    /*!
     * Write number in output stream buffer with \c std::to_chars .\n
     * Floating point numbers are written with the shortest representation which can be read back
     * without precision loss.
     *
     * \tparam T the number type
     *
     * \param value the number to write
     */
    template<typename T>
    void writeNumber(T value);

    /*!
     * Write JSON string: '"', escaped characters, '"'.\n
     * See \ref writeEscaped for escaped characters.
     *
     * \param str the characters to write
     */
    void writeString(std::string_view str);

    /*!
     * Write characters escaped according to RFC 8259 (without '"' around): '"', '\\' and control characters
     * (\c \\b, \c \\f, \c \\n, \c \\r, \c \\t, \c \\u00XX for the others). Other characters (UTF-8 included)
     * are written as is.
     *
     * \param str the characters to write
     */
    void writeEscaped(std::string_view str);

  private:
    /*!
     * Manage indentation according to context
     */
    void indent();

    /*!
     * Write escape sequence of character
     *
     * \param c the character to escape
     */
    void writeEscapeSequence(char c);
    // Methods -

  };

}

#include "json/impl/writerContext.hpp"
