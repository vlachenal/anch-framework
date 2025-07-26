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
#include <optional>
#include <utility>
#include <functional>
#include <stdint.h>

#include "json/mappingOptions.hpp"

namespace anch::json {

  /*!
   * \brief JSON reader context
   *
   * Context is used to store mapping options and deserialization buffers
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class ReaderContext {

    // Attributes +
  public:
    /*! The input stream to parse */
    std::istream& input;

    /*! Mapping options */
    anch::json::MappingOptions options;

    /*! Primitive value buffer */
    std::string pbuf;

    /*! The current offset */
    uint32_t offset;

    /*! Parser buffer size (default to 1024) */
    std::size_t bufferSize;

    /*! Parser buffer */
    char* buffer;

    /*! Global number of read characters */
    std::size_t read;

  private:
    /*! Discard function */
    std::function<char()> _discardFunc;

    /*! Consume unknown field's value */
    std::function<void()> _unknownFunc;

    /*! Check payload size */
    std::function<void(std::streamsize read)> _checkSize;
    // Attributes -

    // Constructors +
  public:
    /*!
     * Forbids \c ReaderContext default constructor
     */
    ReaderContext() = delete;

    /*!
     * Forbids \c ReaderContext copy constructor
     *
     * \param context the context not to copy
     */
    ReaderContext(const ReaderContext& context) = delete;

    /*!
     * Forbids \c ReaderContext move constructor
     *
     * \param context the context not to move
     */
    ReaderContext(ReaderContext&& context) = delete;

    /*!
     * \c ReaderContext constructor
     *
     * \param is the input stream to read
     * \param options the mapping options to use
     */
    ReaderContext(std::istream& is, const anch::json::MappingOptions& options) noexcept;
    // Constructors -

    // Destructor +
  public:
    /*!
     * \c ReaderContext destructor\n
     *
     * Delete buffers
     */
    virtual ~ReaderContext() noexcept;
    // Destructor -

    // Methods +
  public:
    /*!
     * Discard character until found undiscardable character or fail according to options
     *
     * \return the next undiscardable character
     *
     * \throw anch::json::MappingError potential overflow ...
     */
    char discard();

    /*!
     * Parse unknown value or fail according to options
     *
     * \throw anch::json::MappingError unknown fields are not allowed or parsing error
     */
    void parseUnknown();

    /*!
     * Get next character in buffer.\n
     * If buffer has reached its end, it will refill buffer with the input stream.
     *
     * \param current the character to write in
     *
     * \return \c true if buffer end has been reached
     *
     * \throw anch::json::MappingError no more character available
     */
    bool next(char& current);

    /*!
     * Go to previous character.
     */
    void back();

  protected:
    /*!
     * Refill buffer from input stream
     *
     * \return the number of read characters
     */
    virtual std::streamsize refillBuffer();
    // Methods -

  };

}

#include "json/impl/readerContext.hpp"
