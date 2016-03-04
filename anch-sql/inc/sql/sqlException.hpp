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
#ifndef _ANCH_SQL_ERROR_H_
#define _ANCH_SQL_ERROR_H_

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
      // Attributes -

      // Constructors +
    public:
      /*!
       * \ref SqlException constructor
       *
       * \param msg the error message
       */
      SqlException(std::string msg) noexcept: _msg(msg) {
	// Nothing to do
      }
      // Constructors -

      // Destructor +
    public:
      /**!
       * \ref SqlException destructor
       */
      virtual ~SqlException() noexcept {
	// Nothing to do
      }
      // Destructor -

      // Methods +
    public:
      /*!
       * Error message getter
       *
       * \return the error message
       */
      const char* what() const noexcept {
	return _msg.data();
      }
      // Methods -

    };

  }
}

#endif // _ANCH_SQL_ERROR_H_
