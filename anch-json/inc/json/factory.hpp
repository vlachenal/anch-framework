/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
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
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

#include "anch-json-export.hpp"


namespace anch::json {

  /*!
   * \brief Check if type is a primitive JSON type.
   *
   * When not specialzed, the function always return \c false .\n
   * Every \c C basic types are defined as primitive. \c std::string and \c std::string_view are also defined as primitive.\n
   * \n
   * You can declare any type as primitive with template function specialization.
   *
   * \tparam T the type to check
   *
   * \return \c true if type is a JSON primitive type, \c false otherwise
   */
  template<typename T>
  constexpr bool isPrimitive();

  /*!
   * \brief JSON mapper factory
   *
   * Each JSON mapper will have only one instance.
   *
   * \tparam T the object type
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  template<typename T>
  class ANCH_JSON_EXPORT Factory {

  public:
    /*!
     * Get JSON mapper unique instance
     *
     * \return the \ref JSONMapper or the \ref PrimitiveMapper unique instance according to type (constant: fields
     *         are registered once, when the mapper is built from the description of the type)
     */
    static const auto& getInstance();

  };

}  // anch::json

#include "json/impl/factory.hpp"
