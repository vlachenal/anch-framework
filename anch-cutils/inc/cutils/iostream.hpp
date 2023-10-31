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

#include <streambuf>
#include <cstring>
#include <sstream>
#include <functional>

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
   * \c data is optional: when \c NULL , it will be allocated/deallocated by \ref CStreambuf constructor/destructor.
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  struct cbuffer {

    /*! Buffer data */
    char* data;

    /*! Buffer size */
    std::size_t size;

    /*! Read data function */
    std::function<std::size_t(char*,std::size_t)> read = nullptr;

    /*! Write data function */
    std::function<std::size_t(char*,std::size_t)> write = nullptr;

  };

  // Classes declaration for \c friend usage +
  class CIStream;
  class COStream;
  class CIOStream;
  // Classes declaration for \c friend usage -

  /*!
   * C stream buffer implementation
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class CStreambuf: public std::streambuf {

    friend anch::cutils::CIStream;
    friend anch::cutils::COStream;
    friend anch::cutils::CIOStream;

    // Attributes +
  private:
    /*! C buffer data */
    anch::cutils::cbuffer _buffer;

    /*! Delete buffer in destructor flag */
    bool _deleteBuffer;
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
    CStreambuf(anch::cutils::cbuffer cbuffer, anch::cutils::Direction dir);

  protected:
    /*!
     * \ref CStreambuf default protected constructor
     */
    CStreambuf() noexcept;
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref CStreambuf destructor
     */
    virtual ~CStreambuf();
    // Destructor -

    // Methods +
  public:
    /*!
     * Read input stream
     *
     * \return the number of bytes which has been read
     */
    virtual std::streambuf::int_type underflow();

    /*!
     * Write to input stream
     *
     * \param value the byte to write
     *
     * \return the number of bytes which has been write
     */
    virtual std::streambuf::int_type overflow(std::streambuf::int_type value);

    /*!
     * Consume stream ...
     *
     * \return plop
     */
    virtual int sync();

  private:
    /*!
     * Initiliaze buffer
     */
    void initBuffer();
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

    // Constructors +
  public:
    /*!
     * \ref CIStream constructor
     *
     * \param cbuffer the buffer to write into
     *
     * \throw std::invalid_argument buffer does not reach input stream requirements
     */
    CIStream(anch::cutils::cbuffer cbuffer);

  protected:
    /*!
     * \ref CIStream protected default constructor
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
    void setBuffer(anch::cutils::cbuffer cbuffer);
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

    // Constructors +
  public:
    /*!
     * \ref COStream constructor
     *
     * \param cbuffer the buffer to consume
     *
     * \throw std::invalid_argument buffer does not reach output stream requirements
     */
    COStream(anch::cutils::cbuffer cbuffer);

  protected:
    /*!
     * \ref COStream protected default constructor
     */
    COStream();
    // Constructors -


    // Destructor +
  public:
    /*!
     * \ref COStream destructor
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
    void setBuffer(anch::cutils::cbuffer cbuffer);
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

    // Constructors +
  public:
    /*!
     * \ref CIOStream constructor
     *
     * \param cbuffer the buffer to consume
     *
     * \throw std::invalid_argument buffer does not reach in-out-put stream requirements
     */
    CIOStream(anch::cutils::cbuffer cbuffer);

  protected:
    /*!
     * \ref CIOStream protected default constructor
     */
    CIOStream();
    // Constructors -


    // Destructor +
  public:
    /*!
     * \ref CIOStream destructor
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
    void setBuffer(anch::cutils::cbuffer cbuffer);
    // Methods -

  };

}
