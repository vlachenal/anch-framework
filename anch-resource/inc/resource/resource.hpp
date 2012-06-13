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
#ifndef _ANCH_RESOURCE_RESOURCE_H_
#define _ANCH_RESOURCE_RESOURCE_H_

#include <iostream>
#include <map>
#include <mutex>

#include "resource/section.hpp"

namespace anch {
  namespace resource {

    /**
     * Resource manager
     *
     * @author Vincent Lachenal
     */
    class Resource {
    private:
      // Attributes +
      /** Cached resources */
      static std::map<std::string,Resource> RESOURCES;

      /** Mutex */
      static std::mutex MUTEX;

      /** Resources */
      std::map<std::string,anch::resource::Section> _resources;
      // Attributes -

    private:
      // Constructors +
      /**
       * {@link Resource} private constructor
       */
      Resource();
      // Constructors -

    public:
      // Destructor +
      /**
       * {@link Resource} destructor
       */
      virtual ~Resource();
      // Destructor -

    public:
      /**
       * Get Resource instance from file
       *
       * @param filePath The resource file path
       *
       * @return The {@link Resource} unique instance
       */
      static const Resource& getResource(const std::string& filePath);

      /**
       * Get parameter value from its name and section
       *
       * @param value The value to set
       * @param param The parameter to find
       * @param section The parameter section (optional)
       *
       * @return <code>true</code> if value has been found, <code>false</code> otherwise.
       */
      bool getParameter(std::string& value,
			const std::string& param,
			const std::string& section = "") const;

    };

  }
}

#endif // _ANCH_RESOURCE_RESOURCE_H_
