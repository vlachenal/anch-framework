/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
  Copyright (C) 2020 Vincent Lachenal

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

#include <map>
#include <string>
#include <vector>
#include <optional>

namespace anch::rest {

  /*!
   * \brief HTTP headers
   *
   * This class add only formatting functions to map.\n
   * Conversions are done with:
   *  - \c get will call \c find after formatting
   *  - \c has will call \c contains after formatting
   *  - \c put and \c add will call \c insert after formatting
   *
   * Map methods will be masked to ensure headers formatting.
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class Headers: private std::map<std::string,std::vector<std::string>> {
  public:
    /*!
     * Get header
     *
     * \param key the header name to find
     *
     * \return the value(s) when found, \c empty if not found
     */
    std::optional<std::vector<std::string>> get(const std::string& key);

    /*!
     * Get header
     *
     * \param key the header name to find
     *
     * \return the value(s) when found, \c empty if not found
     */
    const std::optional<const std::vector<std::string>> get(const std::string& key) const;

    /*!
     * Check if header has been defined
     *
     * \param key the header name to find
     *
     * \return \c true when header has been found, \c false otherwise
     */
    bool has(const std::string& key) const;

    /*!
     * Insert header if not exists
     *
     * \param name the header name
     * \param values the header values
     *
     * \return \c true when added, \c false otherwise (when header already exists)
     */
    bool add(const std::string& name, const std::vector<std::string>& values);

    /*!
     * Insert header if not exists
     *
     * \param name the header name
     * \param value the header value
     *
     * \return \c true when added, \c false otherwise (when header already exists)
     */
    bool add(const std::string& name, const std::string& value);

    /*!
     * Insert or replace header
     *
     * \param name the header name
     * \param values the header values
     */
    void put(const std::string& name, const std::vector<std::string>& values);

    /*!
     * Insert or replace header
     *
     * \param name the header name
     * \param value the header value
     */
    void put(const std::string& name, const std::string& value);

    /*!
     * Format header
     *
     * \param name the header name
     *
     * \return the formatted value if header exists
     */
    std::optional<std::string> format(const std::string& name) const;

  };

}  // anch::rest
