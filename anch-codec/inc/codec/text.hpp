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

#include "anch-codec-export.hpp"

#include <istream>
#include <ostream>
#include <string>
#include <string_view>

namespace anch::codec {

  /*!
   * \c text/plain codec: serializes values with stream injection operator, deserializes strings.\n
   * It is part of anch-codec (no format library needed) and always available.
   *
   * \author Vincent Lachenal
   *
   * \since 0.3 (\c anch::rest::Text in 0.2)
   */
  struct ANCH_CODEC_EXPORT Text {

    /*!
     * Media type
     *
     * \return \c text/plain
     */
    static std::string_view mediaType() noexcept;

    /*!
     * Write value
     *
     * \param value the value
     * \param out the output stream
     */
    template<typename T>
    requires requires(std::ostream& out, const T& value) { out << value; }
    static void serialize(const T& value, std::ostream& out);

    /*!
     * Read the whole input
     *
     * \param value the value to fill
     * \param input the input stream
     */
    static void deserialize(std::string& value, std::istream& input);

  };

}

#include "codec/impl/text.hpp"
