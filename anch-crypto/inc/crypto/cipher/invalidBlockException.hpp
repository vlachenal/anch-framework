/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
  Copyright (C) 2012 Vincent Lachenal

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

#include <exception>
#include <iostream>

namespace anch {
  namespace crypto {

    /*!
     * \brief Exception on receiving an invalid block.
     *
     * \ref InvalidBlockException can be thrown on decipher a block or a stream.\n
     * Blocks have fixed size. The cipher result will always have complete number of block.
     *
     * \since 0.1
     *
     * \author Vincent Lachenal
     */
    class InvalidBlockException: public std::exception {

      // Attributes +
    private:
      /*! Error message */
      std::string _message;
      // Attributes -

      // Constructors +
    public:
      /*!
       * \ref InvalidBlockException constructor
       *
       * \param message the error message
       */
      InvalidBlockException(const std::string& message);
      // Constructors -

      // Destructor +
    public:
      /*!
       * \ref InvalidBlockException destructor
       */
      virtual ~InvalidBlockException() noexcept;
      // Destructor -

      // Methods +
    public:
      /*!
       * Returns error cause (message).
       *
       * \return the error cause
       */
      virtual const char* what() const noexcept override;
      // Methods -

    };

  }
}
