/*
    This file is part of Configuration resource manager.

    Configuration resource manager is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Configuration resource manager is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Configuration resource manager.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _RESOURCE_SECTION_H_
#define _RESOURCE_SECTION_H_

#include <iostream>
#include <map>

namespace resource {

  /**
   * Parameters defined in a section
   *
   * @author Vincent Lachenal
   */
  class Section {

  private:
    // Attributes +
    /** Parameter default value when not found */
    static std::string DEFAULT_VALUE;

    /** Parameters map */
    std::map<std::string,std::string> _parameters;
    // Attributes -

  public:
    // Constructors +
    /**
     * Section constructor
     */
    Section();
    // Constructors -

    // Destructor +
    /**
     * Section destructor
     */
    ~Section();
    // Destructor -

  public:
    // Accessors +
    /**
     * Parameters getter
     *
     * @return The parameters
     */
    inline const std::map<std::string,std::string>& getParameters() {
      return _parameters;
    };

    /**
     * Parameters getter
     *
     * @param parameters The parameters
     */
    inline void setParameters(const std::map<std::string,std::string>& parameters) {
      _parameters = parameters;
    };

    /**
     * Parameter getter.<br>
     * Search a parameter value by parameter name. If not found, an empty string will be returned.
     *
     * @param parameterName The parameter's name
     *
     * @return The parameter value or empty if not found
     */
    inline const std::string& getParameter(const std::string& parameterName) {
      std::map<std::string,std::string>::iterator iter = _parameters.find(parameterName);
      if(iter == _parameters.end()) {
	return Section::DEFAULT_VALUE;
      } else {
	return iter->second;
      }
    };
    // Accessors -

  };

}

#endif // _RESOURCE_SECTION_H_

