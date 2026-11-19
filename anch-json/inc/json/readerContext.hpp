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

#include "anch-json-export.hpp"

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
  class ANCH_JSON_EXPORT ReaderContext {

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

    /*! Current nesting depth of objects and arrays */
    std::size_t depth;

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
     *
     * \throw anch::json::MappingError input is bigger than \c max_payload option
     */
    ReaderContext(std::istream& is, const anch::json::MappingOptions& options);
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

    /*!
     * Enter object or array: increment nesting depth.\n
     * Use \c DepthGuard to leave level automatically.
     *
     * \throw anch::json::MappingError depth is greater than \c max_depth option (when not \c 0 )
     */
    void enterLevel();

    /*!
     * Leave object or array: decrement nesting depth.
     */
    void leaveLevel() noexcept;

  protected:
    /*!
     * Refill buffer from input stream
     *
     * \return the number of read characters
     *
     * \throw anch::json::MappingError input is bigger than \c max_payload option
     */
    virtual std::streamsize refillBuffer();

    /*!
     * Check that the number of read characters does not exceed \c max_payload option (when not \c 0 )
     *
     * \throw anch::json::MappingError input is bigger than \c max_payload option
     */
    void checkPayloadSize() const;

  private:
    /*!
     * Leave current level and raise maximum depth error (out of line: not on hot path)
     *
     * \throw anch::json::MappingError always
     */
    [[noreturn]] void throwDepthError();
    // Methods -

  };

  /*!
   * \brief Nesting depth guard
   *
   * Enter object or array level on construction and leave it on destruction.
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class ANCH_JSON_EXPORT DepthGuard {

    // Attributes +
  private:
    /*! The reader context */
    anch::json::ReaderContext& _context;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \c DepthGuard constructor
     *
     * \param context the reader context
     *
     * \throw anch::json::MappingError depth is greater than \c max_depth option
     */
    explicit DepthGuard(anch::json::ReaderContext& context);

    /*!
     * Forbids \c DepthGuard copy constructor
     *
     * \param guard the guard not to copy
     */
    DepthGuard(const DepthGuard& guard) = delete;
    // Constructors -

    // Destructor +
  public:
    /*!
     * \c DepthGuard destructor: leave level
     */
    ~DepthGuard() noexcept;
    // Destructor -

  };

}

#include "json/impl/readerContext.hpp"
