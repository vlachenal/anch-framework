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

    /*! Mapping options */
    anch::json::MappingOptions options;

  private:
    /*! First field flag */
    bool _firstField;

    /*! Current number of indentation */
    uint16_t _nbIndent;

    /*! Current indentation to use */
    std::string _indent;

    /*! Write \c null value function */
    std::function<bool(const std::string&)> _nullFunc;

    /*! Next field function */
    std::function<void()> _nextFunc;
    // Attributes -

    // Constructors +
  public:
    /*!
     * Forbids \c WriterContext default constructor
     */
    WriterContext() = delete;

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

  private:
    /*!
     * \c WriterContext copy constructor (private)
     *
     * \param context the context not to copy
     */
    WriterContext(const WriterContext& context);
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
     * \param field the field
     *
     * \return \c true when \c null value has been serialize, \c false otherwise
     */
    bool writeNull(const std::string& field);

    /*!
     * Wirte empty array according to options.\n
     * When serialize, it will be serialized as '"<field>":[]'
     *
     * \param field the field name
     */
    bool writeEmptyArray(const std::string& field);

    /*!
     * Wirte empty object (map) according to options.\n
     * When serialize, it will be serialized as '"<field>":{}'
     *
     * \param field the field name
     */
    bool writeEmptyObject(const std::string& field);

    /*!
     * Write field and add ':' into output stream
     *
     * \param field the field
     */
    void writeField(const std::string& field);

    /*!
     * Write begin array ('[') according to formatter options
     */
    void beginArray();

    /*!
     * Write end array (']') according to formatter options
     */
    void endArray();

    /*!
     * Write begin array ('{') according to formatter options
     */
    void beginObject();

    /*!
     * Write end array ('}') according to formatter options
     */
    void endObject();

    /*!
     * Write field/value delimiter (',') according to formatter options
     */
    void next();
    // Methods -

  };

}
