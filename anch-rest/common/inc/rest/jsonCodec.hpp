/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2026 Vincent Lachenal

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

#include <istream>
#include <ostream>
#include <string_view>

#include "json/json.hpp"

namespace anch::rest {

  /*!
   * \c application/json codec based on anch-json: values need an anch-json mapper
   * (\c anch::json::registerObject ).
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  struct Json {

    /*!
     * Media type
     *
     * \return \c application/json
     */
    static std::string_view mediaType() noexcept;

    /*!
     * Serialize value
     *
     * \param value the value
     * \param out the output stream
     */
    template<typename T>
    static void serialize(const T& value, std::ostream& out);

    /*!
     * Deserialize value
     *
     * \param value the value to fill
     * \param input the input stream
     *
     * \throw anch::json::MappingError on invalid JSON
     */
    template<typename T>
    static void deserialize(T& value, std::istream& input);

  };

}

#include "rest/impl/jsonCodec.hpp"
