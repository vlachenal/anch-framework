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
#ifndef _ANCH_FILE_FILE_EXCEPTIOn_H_
#define _ANCH_FILE_FILE_EXCEPTION_H_

#include <iostream>
#include <exception>

namespace anch {
  namespace file {

    class FileException: public std::exception {
      // Attributes +
    private:
      /** The error message */
      std::string _message;
      // Attributes -


      // Constructors +
    public:
      /**
       * {@link FileException} constructor
       *
       * @param message The error message
       */
      FileException(const std::string& message);

      /**
       * {@link FileException} constructor
       *
       * @param message The error message
       * @param errnum System error number
       */
      FileException(const std::string& message, int errnum);
      // Constructors -

      // Destructor +
      /**
       * {@link FileException} destructor
       */
      virtual ~FileException() noexcept;
      // Destructor -


      // Methods +
      /**
       * Display message error
       *
       * @return The message error
       */
      virtual const char* what() const noexcept;
      // Methods -

    };

  }
}

#endif // _ANCH_FILE_FILE_EXCEPTION_H_
