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
#ifndef _ANCH_DEVICE_EXCEPTION_H_
#define _ANCH_DEVICE_EXCEPTION_H_

#include <iostream>
#include <exception>

namespace anch {
  namespace device {

    /*!
     * Device exception class.
     *
     * \author Vincent Lachenal
     */
    class DeviceException : public std::exception {
    private:
      /*! Exception message */
      std::string _message;

    public:
      // Constructors +
      /*!
       * \ref DeviceException constructor
       *
       * \param message The error message
       */
      DeviceException(const std::string& message) noexcept;

      /*!
       * \ref DeviceException constructor
       *
       * \param message The error message
       * \param errorCode The error code
       */
      DeviceException(const std::string& message, int errorCode) noexcept;
      // Constructors -

      // Destructor +
      /*!
       * \ref DeviceException destructor
       */
      virtual ~DeviceException() noexcept;
      // Destructor -

    public:
      // Methods +
      /*!
       * Return the error message
       *
       * \return The error message
       */
      virtual const char* what() const noexcept;
      // Methods -

    };

  }
}

#endif // _ANCH_DEVICE_EXCEPTION_H_
