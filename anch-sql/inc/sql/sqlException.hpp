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
#include <string>

namespace anch {
  namespace sql {

    /*!
     * \brief SQL exception
     *
     * Manage every SQL error
     * \todo put implementation in cpp file
     *
     * \author Vincent Lachenal
     *
     * \since 0.1
     */
    class SqlException: public std::exception {

      // Attributes +
    private:
      /*! Error message */
      std::string _msg;

      /*! Connection status */
      bool _ok;
      // Attributes -

      // Constructors +
    public:
      /*!
       * \ref SqlException constructor
       *
       * \param msg the error message
       * \param ok connection status
       */
      SqlException(std::string msg, bool ok = true) noexcept;
      // Constructors -

      // Destructor +
    public:
      /**!
       * \ref SqlException destructor
       */
      virtual ~SqlException() noexcept;
      // Destructor -

      // Methods +
    public:
      /*!
       * Error message getter
       *
       * \return the error message
       */
      virtual const char* what() const noexcept;

      /*!
       * Connection status getter
       *
       * \return \c true if connection  can be reused, \c false otherwise
       */
      inline bool isOK() const noexcept {
	return _ok;
      }
      // Methods -

    };

  }
}
