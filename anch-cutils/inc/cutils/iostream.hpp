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

namespace anch::cutils {

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

    /*! Handle data function */
    std::size_t(*handle)(char*,std::size_t);

  };

  /*!
   * C stream buffer implementation
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class CStreambuf: public std::streambuf {

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
     */
    CStreambuf(anch::cutils::cbuffer cbuffer);
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
     */
    CIStream(anch::cutils::cbuffer cbuffer);
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref CIStream destructor
     */
    virtual ~CIStream();
    // Destructor -

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
     */
    COStream(anch::cutils::cbuffer cbuffer);
    // Constructors -


    // Destructor +
  public:
    /*!
     * \ref COStream destructor
     */
    virtual ~COStream();
    // Destructor -

  };

}
