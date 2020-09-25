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


namespace anch {
  namespace json {

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
    class JSONFactory {

    public:
      /*!
       * Get JSON mapper unique instance
       *
       * \return the \ref JSONMapper or the \ref JSONPrimitiveMapper unique instance according to type
       */
      static auto& getInstance();

    };

  }  // json
}  // anch

#include "json/impl/factory.hpp"
