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

#include <cstddef>
#include <functional>
#include <istream>
#include <memory>
#include <ostream>
#include <streambuf>

namespace anch::cutils {

  /*!
   * Stream direction enum
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  enum class Direction {

    /*! in */
    IN = 0b01,

    /*! out */
    OUT = 0b10,

    /*! inout */
    INOUT = 0b11,

  };

  /*!
   * \brief C buffer
   *
   * Read and write functions return the number of bytes which have been read or written:
   * - read: \c 0 means end of stream ;
   * - write: it can write less bytes than requested (it is called again with the remaining bytes) ; \c 0 means error.
   *
   * They can throw exceptions: the stream sets its \c badbit (and rethrows according to its exceptions mask).\n
   * \n
   * \c data is optional: when \c NULL , it will be allocated/deallocated by \ref CStreambuf. When set, it has to
   * contain at least \c size bytes and to outlive the stream. In input-output mode, it is used for reading and
   * a writing area of \c size bytes is allocated.
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  struct cbuffer {

    /*! Buffer data */
    char* data = nullptr;

    /*! Buffer size (has to be greater than 0) */
    std::size_t size = 0;

    /*! Read data function */
    std::function<std::size_t(char*,std::size_t)> read = nullptr;

    /*! Write data function */
    std::function<std::size_t(const char*,std::size_t)> write = nullptr;

  };

  /*!
   * C stream buffer implementation.\n
   * \n
   * Reading and writing areas are distinct. Pending written data are sent before reading new data (as a tied
   * stream): request / response protocols do not need an explicit flush.
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class CStreambuf: public std::streambuf {

    // Attributes +
  private:
    /*! Read data function (\c nullptr when not readable) */
    std::function<std::size_t(char*,std::size_t)> _read;

    /*! Write data function (\c nullptr when not writable) */
    std::function<std::size_t(const char*,std::size_t)> _write;

    /*! Areas size */
    std::size_t _size;

    /*! Owned memory (areas which have not been provided) */
    std::unique_ptr<char[]> _owned;

    /*! Reading area */
    char* _in;

    /*! Writing area */
    char* _out;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref CStreambuf constructor
     *
     * \param cbuffer the \ref cbuffer to user
     * \param dir stream direction
     *
     * \throw std::invalid_argument \c cbuffer definition according to \c dir
     */
    CStreambuf(const anch::cutils::cbuffer& cbuffer, anch::cutils::Direction dir);

    /*!
     * \ref CStreambuf default constructor: \ref setBuffer has to be called before use
     */
    CStreambuf() noexcept;

    /*!
     * Prohibits \ref CStreambuf copy constructor
     */
    CStreambuf(const CStreambuf&) = delete;
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref CStreambuf destructor.\n
     * Send pending written data (errors are ignored).
     */
    virtual ~CStreambuf();
    // Destructor -

    // Operators +
  public:
    /*!
     * Prohibits \ref CStreambuf copy assignment
     */
    CStreambuf& operator=(const CStreambuf&) = delete;
    // Operators -

    // Methods +
  public:
    /*!
     * Set (or replace) buffer.\n
     * Pending written data are sent before.
     *
     * \param cbuffer the \ref cbuffer to user
     * \param dir stream direction
     *
     * \throw std::invalid_argument \c cbuffer definition according to \c dir
     */
    void setBuffer(const anch::cutils::cbuffer& cbuffer, anch::cutils::Direction dir);

  protected:
    /*!
     * Read data from read function when reading area is empty
     *
     * \return the next character or \c eof
     */
    virtual std::streambuf::int_type underflow() override;

    /*!
     * Send writing area content when it is full
     *
     * \param value the character to add (or \c eof)
     *
     * \return \c eof on error, another value otherwise
     */
    virtual std::streambuf::int_type overflow(std::streambuf::int_type value) override;

    /*!
     * Send writing area content
     *
     * \return \c 0 on success, \c -1 on error
     */
    virtual int sync() override;

  private:
    /*!
     * Send writing area content (non virtual: used by destructor)
     *
     * \return \c true on success, \c false on error (unsent data are dropped)
     */
    bool flushOutput();
    // Methods -

  };

  /*!
   * C input stream buffer
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class CIStream: public std::istream {

    // Attributes +
  private:
    /*! Stream buffer */
    anch::cutils::CStreambuf _streambuf;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref CIStream constructor
     *
     * \param cbuffer the buffer to write into
     *
     * \throw std::invalid_argument buffer does not reach input stream requirements
     */
    CIStream(const anch::cutils::cbuffer& cbuffer);

  protected:
    /*!
     * \ref CIStream protected default constructor: \ref setBuffer has to be called before use
     */
    CIStream();
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref CIStream destructor
     */
    virtual ~CIStream();
    // Destructor -

    // Methods +
  protected:
    /*!
     * Set stream buffer
     *
     * \param cbuffer the buffer to use
     *
     * \throw std::invalid_argument buffer does not reach input stream requirements
     */
    void setBuffer(const anch::cutils::cbuffer& cbuffer);
    // Methods -

  };

  /*!
   * C output stream buffer
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class COStream: public std::ostream {

    // Attributes +
  private:
    /*! Stream buffer */
    anch::cutils::CStreambuf _streambuf;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref COStream constructor
     *
     * \param cbuffer the buffer to consume
     *
     * \throw std::invalid_argument buffer does not reach output stream requirements
     */
    COStream(const anch::cutils::cbuffer& cbuffer);

  protected:
    /*!
     * \ref COStream protected default constructor: \ref setBuffer has to be called before use
     */
    COStream();
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref COStream destructor.\n
     * Send pending data.
     */
    virtual ~COStream();
    // Destructor -

    // Methods +
  protected:
    /*!
     * Set stream buffer
     *
     * \param cbuffer the buffer to use
     *
     * \throw std::invalid_argument buffer does not reach output stream requirements
     */
    void setBuffer(const anch::cutils::cbuffer& cbuffer);
    // Methods -

  };

  /*!
   * C input and output stream buffer
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class CIOStream: public std::iostream {

    // Attributes +
  private:
    /*! Stream buffer */
    anch::cutils::CStreambuf _streambuf;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref CIOStream constructor
     *
     * \param cbuffer the buffer to consume
     *
     * \throw std::invalid_argument buffer does not reach in-out-put stream requirements
     */
    CIOStream(const anch::cutils::cbuffer& cbuffer);

  protected:
    /*!
     * \ref CIOStream protected default constructor: \ref setBuffer has to be called before use
     */
    CIOStream();
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref CIOStream destructor.\n
     * Send pending data.
     */
    virtual ~CIOStream();
    // Destructor -

    // Methods +
  protected:
    /*!
     * Set stream buffer
     *
     * \param cbuffer the buffer to use
     *
     * \throw std::invalid_argument buffer does not reach in-out-put stream requirements
     */
    void setBuffer(const anch::cutils::cbuffer& cbuffer);
    // Methods -

  };

}
