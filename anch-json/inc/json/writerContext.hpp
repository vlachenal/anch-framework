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

#include <ostream>
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
    // Methods -

  };

}
