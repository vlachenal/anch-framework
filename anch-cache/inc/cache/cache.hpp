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

#include <optional>


namespace anch::cache {

  /*!
   * \brief \c Cache virtual class
   *
   * Cache cache which manages PUT, GET and DELETE data in cache
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  template<typename Derived>
  class Cache {

  public:
    /*!
     * Put/erase value in cache indexed by key
     *
     * \tparam T the value type
     *
     * \param key the key
     * \param value the value
     */
    template<typename T>
    virtual void put(const std::string& key, const T& value);

    /*!
     * Get value from key
     *
     * \tparam T the value type
     *
     * \param key the key
     *
     * \return the value if found, \c empty otherwise
     */
    template<typename T>
    virtual std::optional<T> get(const std::string& key);

    /*!
     * Remove key/value from cache
     *
     * \param key the key to remove
     */
    virtual void remove(const std::string& key) = 0;

  protected:
    /*!
     * Store object in cache
     *
     * \param key the key
     * \param value the value
     */
    virtual store(const std::string& key, const std::string& value) = 0;

    /*!
     * Retrieve object from cache
     *
     * \param key the object key
     *
     * \return the \c std::string reprensentation of the object
     */
    virtual std::optional<std::string> retrieve(const std::string& key) = 0;

    /*!
     * Delete object from cache
     *
     * \param key the object the remove
     */
    virtual void purge(const std::string& key) = 0;

  };

}  // anch::cache
