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
#ifndef _ANCH_RESOURCE_RESOURCE_H_
#define _ANCH_RESOURCE_RESOURCE_H_

#include <map>
#include <mutex>

#ifdef ANCH_STD_OTP
#include <optional>
#endif

#include "resource/section.hpp"

namespace anch {
  namespace resource {

    /*!
     * Resource manager
     *
     * \author Vincent Lachenal
     */
    class Resource {
    private:
      // Attributes +
      /*! Cached resources */
      static std::map<std::string,Resource> RESOURCES;

      /*! Mutex */
      static std::mutex MUTEX;

      /*! Resources */
      std::map<std::string,anch::resource::Section> _resources;
      // Attributes -

    private:
      // Constructors +
      /*!
       * \ref Resource private constructor
       */
      Resource();
      // Constructors -

    public:
      // Destructor +
      /*!
       * \ref Resource destructor
       */
      virtual ~Resource();
      // Destructor -

    public:
      /*!
       * Get Resource instance from file
       *
       * \param filePath The resource file path
       *
       * \return The \ref Resource unique instance
       */
      static const Resource& getResource(const std::string& filePath);

      /*!
       * Get parameter value from its name and section
       *
       * \param value The value to set
       * \param param The parameter to find
       * \param section The parameter section (optional)
       *
       * \return \c true if value has been found, \c false otherwise.
       */
      bool getParameter(std::string& value,
			const std::string& param,
			const std::string& section = "") const;

#ifdef ANCH_STD_OTP
      /*!
       * Access section
       *
       * \param section the section name
       *
       * \return the optional result
       */
      std::optional<anch::resource::Section> section(const std::string& section) const;

      /*!
       * Access parameter without section
       *
       * \param param the parameter name
       *
       * \return the optional result
       */
      std::optional<std::string> parameter(const std::string& param) const;
#endif

      /*!
       * Get resource configuration
       *
       * \return The configuration
       */
      const std::map<std::string,anch::resource::Section>& getConfiguration() const;

    };

    inline const std::map<std::string,anch::resource::Section>& Resource::getConfiguration() const {
      return _resources;
    }
  }
}

#endif // _ANCH_RESOURCE_RESOURCE_H_
