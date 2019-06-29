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

#include <iostream>
#include <exception>

namespace anch {
  namespace network {

    /*!
     * Network input/output error management by exception
     *
     * \author Vincent Lachenal
     */
    class IOException : public std::exception {
    private:
      /*! Error message */
      std::string _message;

    public:
      // Constructors +
      /*!
       * \ref IOException constructor
       *
       * \param message The error message
       */
      IOException(const std::string& message);

      /*!
       * \ref IOException constructor
       *
       * \param message The error message
       * \param errorCode The error code (from \c getinfoaddr)
       */
      IOException(const std::string& message, int errorCode);
      // Constructors -

      // Destructor +
      /*!
       * \ref IOException destructor
       */
      virtual ~IOException() noexcept;
      // Destructor -

    public:
      /*!
       * Return the error message
       *
       * \return The error message
       */
      virtual const char* what() const noexcept;

    };

  }
}
