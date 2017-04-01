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
#ifndef _ANCH_RESOURCE_SECTION_H_
#define _ANCH_RESOURCE_SECTION_H_

#include <map>

namespace anch {
  namespace resource {

    /*!
     * Parameters defined in a section
     *
     * \author Vincent Lachenal
     */
    class Section {

    public:
      // Attributes +
      /*! Parameter default value when not found */
      static std::string DEFAULT_VALUE;

    private:
      /*! Parameters map */
      std::map<std::string,std::string> _parameters;
      // Attributes -

    public:
      // Constructors +
      /*!
       * \ref Section default constructor
       */
      Section();

      /*!
       * \ref Section copy constructor
       */
      Section(const Section& section);
      // Constructors -

      // Destructor +
      /*!
       * \ref Section destructor
       */
      ~Section();
      // Destructor -

    public:
      // Accessors +
      /*!
       * Parameters getter
       *
       * \return The parameters
       */
      inline const std::map<std::string,std::string>& getParameters() const {
	return _parameters;
      }

      /*!
       * Parameters setter
       *
       * \param parameters The parameters
       */
      inline void setParameters(const std::map<std::string,std::string>& parameters) {
	_parameters = parameters;
      }

      /*!
       * Add parameter to section
       *
       * \param param The parameter
       * \param value The associated value
       */
      inline void addParameter(const std::string& param, const std::string& value) {
	_parameters[param] = value;
      }

      /*!
       * Parameter getter.\n
       * Search a parameter value by parameter name. If not found, an empty string will be returned.
       *
       * \param parameterName The parameter's name
       *
       * \return The parameter value or empty if not found
       */
      inline const std::string& getParameter(const std::string& parameterName) const {
	auto iter = _parameters.find(parameterName);
	if(iter == _parameters.end()) {
	  return Section::DEFAULT_VALUE;
	} else {
	  return iter->second;
	}
      }
      // Accessors -

    };

  }
}

#endif // _ANCH_RESOURCE_SECTION_H_
