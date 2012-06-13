/*
    This file is part of ANCH Framework.

    ANCH Framework is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ANCH Framework is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _ANCH_RESOURCE_FILE_RESOURCE_FILE_EXCEPTION_H_
#define _ANCH_RESOURCE_FILE_RESOURCE_FILE_EXCEPTION_H_

#include <iostream>

namespace anch {
  namespace resource {
    namespace file {

      /**
       * File not found exception
       *
       * @author Vincent Lachenal
       */
      class ResourceFileException: public std::exception {

      private:
	// Attributes +
	/** Exception message */
	std::string _message;
	// Attributes -

      public:
	// Constructors +
	/**
	 * {@link ResourceFileException} constructor
	 *
	 * @param filePath The file path
	 * @param error The error
	 */
	ResourceFileException(const std::string& filePath,
			      const std::string& error);
	// Constructors -

	// Destructor +
	/**
	 * {@link ResourceFileException} destructor
	 */
	virtual ~ResourceFileException() throw();
	// Destructor -

      public:
	/**
	 * Retrieve the complete error message
	 *
	 * @return The error message
	 */
	const char* what() const throw();

      };

    }
  }
}

#endif // _ANCH_RESOURCE_FILE_RESOURCE_FILE_EXCEPTION_H_
