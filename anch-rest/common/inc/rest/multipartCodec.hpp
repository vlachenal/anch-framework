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
   * \c multipart/form-data codec for \ref Form (RFC 7578), parts in memory.\n
   * \n
   * Writing: a random boundary is chosen by \ref contentType (it does not appear in parts), names and
   * file names are escaped as browsers do (double quote, CR and LF are percent-encoded), \c Content-Type is
   * written for files and for fields which are not \c text/plain , other part headers are written as
   * they are (CR and LF are refused).\n
   * \n
   * Reading: \c boundary parameter is required, delimiters end with CRLF, preamble and epilogue are
   * ignored, close delimiter is required. Each part needs a <tt>Content-Disposition: form-data</tt>
   * header with a \c name ; \c filename* (RFC 5987, UTF-8) has priority over \c filename . Part
   * \c Content-Type is \c text/plain by default, \c application/octet-stream for files. Other part
   * headers are kept ( \c Content-Transfer-Encoding is not applied). Malformed bodies raise
   * \c std::invalid_argument ( \c 400 on server side), bodies over \ref limits raise
   * \ref ContentTooLarge ( \c 413 ).
   * \code{.cpp}
api.post("/pets/{id}/photo", [](uint64_t id, anch::rest::Body<anch::rest::Form> form) {
  const anch::rest::Part* photo = form->part("photo");
  ...
}).consumes<anch::rest::Multipart>();

anch::rest::Form form;
form.add("description", "Rex").addFile("photo", "rex.png", anch::rest::MediaType("image", "png"), content);
client.post("/pets/{id}/photo", 42).body<anch::rest::Multipart>(form).send();
   * \endcode
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  struct Multipart {

    /*!
     * Media type
     *
     * \return \c multipart/form-data
     */
    static std::string_view mediaType() noexcept;

    /*!
     * Get limits of a side, to change before handling requests. Default: 16 MiB body, 1000 parts,
     * 16 MiB part, 16 KiB part headers.
     *
     * \param side the side
     *
     * \return the limits
     */
    static anch::rest::FormLimits& limits(anch::rest::Side side);

    /*!
     * Get body media type: \c multipart/form-data with a random boundary which does not appear in
     * form
     *
     * \param form the form
     *
     * \return the media type
     */
    static anch::rest::MediaType contentType(const anch::rest::Form& form);

    /*!
     * Write form
     *
     * \param form the form
     * \param out the output stream
     * \param contentType the body media type (with its boundary)
     * \param side the side which writes form
     *
     * \throw std::invalid_argument when boundary is missing or invalid, when it appears in a part or
     *        when a part header has CR or LF
     */
    static void serialize(const anch::rest::Form& form, std::ostream& out, const anch::rest::MediaType& contentType,
			  anch::rest::Side side);

    /*!
     * Read form
     *
     * \param form the form to fill
     * \param input the input stream
     * \param contentType the body media type (with its boundary)
     * \param side the side which reads form
     *
     * \throw std::invalid_argument when body is malformed
     * \throw ContentTooLarge when body is over \ref limits
     */
    static void deserialize(anch::rest::Form& form, std::istream& input, const anch::rest::MediaType& contentType,
			    anch::rest::Side side);

  };

}
