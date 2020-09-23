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

#include <ostream>
#include <string>

#include "json/mapper.hpp"


namespace anch {
  namespace json {

    /*!
     * Fields registry function\n
     * This function has to be specialized for each class/structure which has to be serialized/deserialized.
     *
     * \tparam T the object type
     *
     * \param mapper the \ref JSONMapper to use
     */
    template<typename T>
    void registerFields(anch::json::JSONMapper<T>& mapper);

    /*!
     * Serialize object
     *
     * \tparam T the object type
     *
     * \param value the object to serialize
     * \param out the output stream write in
     */
    template<typename T>
    void serialize(const T& value, std::ostream& out);

    /*!
     * Serialize object
     *
     * \tparam T the object type
     *
     * \param value the object to serialize
     *
     * \return the serialized object as \c std::string
     */
    template<typename T>
    std::string serialize(const T& value);

  }  // json
}  // anch

#include "json/impl/json.hpp"
