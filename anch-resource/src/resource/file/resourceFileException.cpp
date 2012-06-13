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
#include "resource/file/resourceFileException.hpp"

using std::string;

using anch::resource::file::ResourceFileException;


/**
 * {@link ResourceFileException} constructor
 *
 * @param filePath The file path
 * @param error The error
 */
ResourceFileException::ResourceFileException(const std::string& filePath,
					     const std::string& error) {
  _message = string("Error while on ") + filePath
    + string(" treatment: ") + error;
}

/**
 * {@link ResourceFileException} destructor
 */
ResourceFileException::~ResourceFileException() throw() {
  // Nothing to do
}

/**
 * Retrieve the complete error message
 *
 * @return The error message
 */
const char*
ResourceFileException::what() const throw() {
  return _message.c_str();
}
