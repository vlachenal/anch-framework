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

#include "anch-rest-common-export.hpp"

#include <istream>
#include <ostream>
#include <string>
#include <string_view>

#include "codec/codec.hpp"
#include "codec/mediaType.hpp"

#include "rest/form.hpp"
#include "rest/formMapper.hpp"

namespace anch::rest {

  /*!
   * \c application/x-www-form-urlencoded codec for \ref Form (fields only).\n
   * \n
   * Values are written with the WHATWG URL-encoded serializer (space is \c + ). On reading, \c + is
   * a space, empty pairs are ignored and a pair without \c = is a field with an empty value. Content
   * is UTF-8: another \c charset is refused.\n
   * \n
   * Types with a \ref FormMapper are read and written through a \ref Form .
   * \snippet{trimleft} anch-rest/examples/server.cpp form server
   *
   * \snippet{trimleft} anch-rest/examples/client.cpp form client
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  struct ANCH_REST_COMMON_EXPORT FormUrlEncoded {

    /*!
     * Media type
     *
     * \return \c application/x-www-form-urlencoded
     */
    static std::string_view mediaType() noexcept;

    /*!
     * Get limits of a side, to change before handling requests. Default: 2 MiB body, 1000 fields.
     *
     * \param side the side
     *
     * \return the limits
     */
    static anch::rest::FormLimits& limits(anch::codec::Side side);

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
     * \throw ContentTooLarge when body is over \ref limits
     */
    static void deserialize(anch::rest::Form& form, std::istream& input, const anch::codec::MediaType& contentType,
			    anch::codec::Side side);

    /*!
     * Write value mapped with its \ref FormMapper
     *
     * \tparam T the value type
     *
     * \param value the value
     * \param out the output stream
     * \param side the side which writes value
     *
     * \throw std::invalid_argument when form has a file part
     */
    template<anch::rest::FormMapped T>
    static void serialize(const T& value, std::ostream& out, anch::codec::Side side);

    /*!
     * Read value mapped with its \ref FormMapper
     *
     * \tparam T the value type
     *
     * \param value the value to fill
     * \param input the input stream
     * \param contentType the body media type
     * \param side the side which reads value
     */
    template<anch::rest::FormMapped T>
    static void deserialize(T& value, std::istream& input, const anch::codec::MediaType& contentType, anch::codec::Side side);

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

#include "rest/impl/formCodec.hpp"
