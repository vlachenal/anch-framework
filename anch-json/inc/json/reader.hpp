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

#include <istream>
#include <string>
#include <any>

#include "json/readerContext.hpp"
#include "json/mappingError.hpp"
#include "events/observable.hpp"

namespace anch::json {

  /*!
   * Reader found element type
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  enum class EventType {

    /*! 'null' value (no value) */
    VNULL = 0,

    /*! Boolean 'true' value (no value) */
    TRUE,

    /*! Boolean 'false' value (no value) */
    FALSE,

    /*! Field value (value stored as \ref std::string) */
    FIELD,

    /*! Number value (value stored as \c double) */
    NUMBER,

    /*! String value (value as stored \ref std::string) */
    STRING,

    /*! Begin object (no value) */
    BEGIN_OBJECT,

    /*! End object (no value) */
    END_OBJECT,

    /*! Begin array (no value) */
    BEGIN_ARRAY,

    /*! End array (no value) */
    END_ARRAY

  };

  /*!
   * JSON event
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class JSONItem {

    // Attributes +
  private:
    /*! Event type */
    anch::json::EventType _type;

    /*! Found value if relevant */
    std::any _value;
    // Attributes -

    // Constructors +
  public:
    /*!
     * Forbids \ref JSONEvent default constructor
     */
    JSONItem() = delete;

    /*!
     * \ref JSONEvent constructor
     *
     * \param type the event type
     * \param context the context for reading found value when needed
     */
    JSONItem(anch::json::EventType type, anch::json::ReaderContext& context);
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref JSONEvent destructor
     */
    virtual ~JSONItem() noexcept;
    // Destructor -

    // Accessors +
  public:
    /*!
     * Event type getter
     *
     * \return the type
     */
    anch::json::EventType getType() const;

    /*!
     * Value getter
     *
     * \return the value (can have no value)
     */
    const std::any& getValue() const;
    // Accessors -

  };

  /*!
   * \brief JSON reader based on events.
   *
   * Context is used to store mapping options and deserialization buffers.\n
   * On found JSON elements, observers will be notified with \ref anch::json::JSONItem .\n
   * On parsing error, observers will be notified with \ref anch::json::MappingError .
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class Reader:
    public anch::events::Observable<anch::json::JSONItem>,
    public anch::events::Observable<std::streamsize>,
    public anch::events::Observable<anch::json::MappingError>,
    private anch::json::ReaderContext {

    // Constructors +
  public:
    /*!
     * Forbids \c Reader default constructor
     */
    Reader() = delete;

    /*!
     * Forbids \c Reader copy constructor
     *
     * \param reader the reader not to copy
     */
    Reader(const Reader& reader) = delete;

    /*!
     * Forbids \c Reader move constructor
     *
     * \param reader the reader not to move
     */
    Reader(Reader&& reader) = delete;

    /*!
     * \c Reader constructor
     *
     * \param is the input stream to read
     * \param options the mapping options to use
     *
     * \throw anch::json::MappingError fail to read input stream
     */
    Reader(std::istream& is, const anch::json::MappingOptions& options);
    // Constructors -

    // Destructor +
  public:
    /*!
     * \c Reader destructor\n
     *
     * Delete buffers
     */
    virtual ~Reader();
    // Destructor -

    // Methods +
  public:
    /*!
     * Parse input stream\n
     * Parsing error while be propagate through \ref anch::json::MappingError 's observers
     */
    void parse() noexcept;

    /*!
     * Observalble \c JSONItem cast utility method
     *
     * \return this as observable \c JSONItem
     */
    anch::events::Observable<JSONItem>& itemObs();

    /*!
     * Observalble \c streamsize cast utility method
     *
     * \return this as observable \c streamsize
     */
    anch::events::Observable<std::streamsize>& sizeObs();

    /*!
     * Observalble \c anch::json::MappingError cast utility method
     *
     * \return this as observable \c anch::json::MappingError
     */
    anch::events::Observable<anch::json::MappingError>& errorObs();

  protected:
    /*!
     * Refill buffer from input stream.\n
     * Notify observers with number of read characters.
     *
     * \return the number of read characters
     */
    virtual std::streamsize refillBuffer();

  private:
    /*!
     * Parse field
     */
    void parseField();

    /*!
     * Parse complex object
     */
    void parseObject();

    /*!
     * Parse value
     */
    void parseValue();

    /*!
     * Parse array
     */
    void parseArray();
    // Methods -

  };

}

#include "json/impl/reader.hpp"
