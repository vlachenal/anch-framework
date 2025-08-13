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
#include <ostream>
#include <functional>
#include <list>
#include <set>
#include <map>

#include "json/genericMapper.hpp"
#include "json/constants.hpp"
#include "json/mappingOptions.hpp"


namespace anch::json {

  // JSON mapper early declaration
  template<typename T>
  class Factory;

  /*!
   * \brief JSON primitive types mapper
   *
   * JSON primitive types does not have any fields.
   *
   * \tparam T the object type
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  template<typename T>
  class PrimitiveMapper: public anch::json::GenericMapper<PrimitiveMapper<T>, T> {
    friend anch::json::Factory<T>;

    // Constructors +
  private:
    /*!
     * \ref PrimitiveMapper default private constructor
     */
    PrimitiveMapper();

  public:
    /*!
     * Forbids \ref PrimitiveMapper copy constructor
     *
     * \param other the other mapper not to copy
     */
    PrimitiveMapper(const PrimitiveMapper& other) = delete;

    /*!
     * Forbids \ref PrimitiveMapper move constructor
     *
     * \param other the other mapper not to move
     */
    PrimitiveMapper(PrimitiveMapper&& other) = delete;
    // Constructors -

    // Destructor +
  private:
    /*!
     * \ref PrimitiveMapper private destructor
     */
    virtual ~PrimitiveMapper();
    // Destructor -

    // Methods +
  public:
    /*!
     * Serialize JSON value
     *
     * \param value the value to serialize
     * \param context the writer context
     */
    void serializeValue(const T& value, anch::json::WriterContext& context) const;

    /*!
     * Deserialize JSON value
     *
     * \param value the value to set
     * \param context the reader context
     *
     * \return \false when value is \c null , \c false otherwise
     */
    bool deserializeValue(T& value, anch::json::ReaderContext& context) const;
    // Methods -

  };

}  // anch::json
