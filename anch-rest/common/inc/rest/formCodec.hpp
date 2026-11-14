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
#include <string>
#include <string_view>

#include "rest/codec.hpp"
#include "rest/form.hpp"
#include "rest/mediaType.hpp"

namespace anch::rest {

  /*!
   * \c application/x-www-form-urlencoded codec for \ref Form (fields only).\n
   * \n
   * Values are written with the WHATWG URL-encoded serializer (space is \c + ). On reading, \c + is
   * a space, empty pairs are ignored and a pair without \c = is a field with an empty value. Content
   * is UTF-8: another \c charset is refused.
   * \code{.cpp}
api.post("/login", [](anch::rest::Body<anch::rest::Form> form) {
  std::string user = anch::rest::requiredFormField<std::string>(*form, "user");
  ...
}).consumes<anch::rest::FormUrlEncoded>();

client.post("/login").body<anch::rest::FormUrlEncoded>(anch::rest::Form().add("user", "rex")).send();
   * \endcode
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  struct FormUrlEncoded {

    /*!
     * Media type
     *
     * \return \c application/x-www-form-urlencoded
     */
    static std::string_view mediaType() noexcept;

    /*!
     * Write form
     *
     * \param form the form
     * \param out the output stream
     *
     * \throw std::invalid_argument when form has a file part
     */
    static void serialize(const anch::rest::Form& form, std::ostream& out);

    /*!
     * Read form
     *
     * \param form the form to fill
     * \param input the input stream
     * \param contentType the body media type
     * \param side the side which reads form
     *
     * \throw std::invalid_argument when encoding is invalid or when charset is not UTF-8
     */
    static void deserialize(anch::rest::Form& form, std::istream& input, const anch::rest::MediaType& contentType,
			    anch::rest::Side side);

    /*!
     * Encode value with the WHATWG URL-encoded serializer: ASCII alphanumeric characters and
     * \c *-._ are kept, space is \c + , other bytes are percent-encoded
     *
     * \param value the value
     *
     * \return the encoded value
     */
    static std::string encode(std::string_view value);

  };

}
