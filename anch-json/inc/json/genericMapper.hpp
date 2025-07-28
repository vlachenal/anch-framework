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

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <list>
#include <set>
#include <map>

#include "json/readerContext.hpp"

namespace anch::json {

  /*!
   * \brief Generic mapper
   *
   * Based on C++ CRT. It provides generic functions for collections mapping.
   *
   * \param M the inherited class type (mapper)
   * \param T the type to map
   *
   * \author Vincent Lachenal
   */
  template<typename M, typename T>
  class GenericMapper {

    // Constructors +
  protected:
    /*!
     * \ref GenericMapper defaullt protected constructor
     */
    GenericMapper();

  public:
    /*!
     * Forbids \ref GenericMapper copy constructor
     *
     * \param other the \ref GenericMapper not to copy
     */
    GenericMapper(const GenericMapper<M,T>& other) = delete;

    /*!
     * Forbids \ref GenericMapper move constructor
     *
     * \param other the \ref GenericMapper not to move
     */
    GenericMapper(GenericMapper<M,T>&& other) = delete;
    // Constructors -

    // Destructor +
  protected:
    /*!
     * \ref GenericMapper destructor
     */
    virtual ~GenericMapper();
    // Destructor -

    // Methods +
  public:
    /*!
     * Deserialize JSON value
     *
     * \param value the value to set
     * \param context the mapping context
     *
     * \return \false when value is \c null , \c false otherwise
     */
    bool deserialize(std::optional<T>& value, anch::json::ReaderContext& context) const;

    /*!
     * Deserialize JSON value
     *
     * \param value the value to set
     * \param context the mapping context
     *
     * \return \false when value is \c null , \c false otherwise
     */
    bool deserialize(T* value, anch::json::ReaderContext& context) const;

    /*!
     * Deserialize JSON value
     *
     * \param value the value to set
     * \param context the mapping context
     */
    bool deserialize(std::vector<T>& value, anch::json::ReaderContext& context) const;

    /*!
     * Deserialize JSON value
     *
     * \param value the value to set
     * \param context the mapping context
     */
    bool deserialize(std::list<T>& value, anch::json::ReaderContext& context) const;

    /*!
     * Deserialize JSON value
     *
     * \param value the value to set
     * \param context the mapping context
     */
    bool deserialize(std::set<T>& value, anch::json::ReaderContext& context) const;

    /*!
     * Deserialize JSON value
     *
     * \param value the value to set
     * \param context the mapping context
     */
    bool deserialize(std::map<std::string,T>& value, anch::json::ReaderContext& context) const;
    // Methods -

  };

}

#include "json/impl/genericMapper.hpp"
