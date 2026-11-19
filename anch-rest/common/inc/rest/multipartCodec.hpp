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

#include "rest/codec.hpp"
#include "rest/form.hpp"
#include "rest/formMapper.hpp"
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
   * Parts larger than \ref FormLimits::maxMemoryPartSize are read in temporary files (deleted with their
   * last \ref Part copy) and file parts are written from their file: large bodies do not need memory.
   * The client sends bodies with \ref stream .\n
   * \n
   * Reading: \c boundary parameter is required, delimiters end with CRLF, preamble and epilogue are
   * ignored, close delimiter is required. Each part needs a <tt>Content-Disposition: form-data</tt>
   * header with a \c name ; \c filename* (RFC 5987, UTF-8) has priority over \c filename . Part
   * \c Content-Type is \c text/plain by default, \c application/octet-stream for files. Other part
   * headers are kept ( \c Content-Transfer-Encoding is not applied). Malformed bodies raise
   * \c std::invalid_argument ( \c 400 on server side), bodies over \ref limits raise
   * \ref ContentTooLarge ( \c 413 ).\n
   * \n
   * Types with a \ref FormMapper are read and written through a \ref Form .
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
  struct ANCH_REST_COMMON_EXPORT Multipart {

    /*!
     * Media type
     *
     * \return \c multipart/form-data
     */
    static std::string_view mediaType() noexcept;

    /*!
     * Get limits of a side, to change before handling requests. Default: 16 MiB body, 1000 parts,
     * 16 MiB part, 16 KiB part headers, 1 MiB part in memory (larger parts are written in temporary
     * files).
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
     * Get body media type of a value mapped with its \ref FormMapper : \c multipart/form-data with a
     * random boundary (\ref serialize refuses it if it appears in a part)
     *
     * \tparam T the value type
     *
     * \param value the value
     *
     * \return the media type
     */
    template<anch::rest::FormMapped T>
    static anch::rest::MediaType contentType(const T& value);

    /*!
     * Get random boundary: \c "anch-" and 32 alphanumeric characters
     *
     * \return the boundary
     */
    static std::string randomBoundary();

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
     * Get form body as a stream: parts are read when the stream is read (files are not loaded in
     * memory), body size is known
     *
     * \param form the form (copied in the stream, parts files are shared)
     * \param contentType the body media type (with its boundary)
     * \param side the side which writes form
     *
     * \return the body stream and its size
     *
     * \throw std::invalid_argument when boundary is missing or invalid, when it appears in a part in
     *        memory or when a part header has CR or LF
     */
    static anch::rest::BodyStream stream(const anch::rest::Form& form, const anch::rest::MediaType& contentType,
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

    /*!
     * Write value mapped with its \ref FormMapper
     *
     * \tparam T the value type
     *
     * \param value the value
     * \param out the output stream
     * \param contentType the body media type (with its boundary)
     * \param side the side which writes value
     */
    template<anch::rest::FormMapped T>
    static void serialize(const T& value, std::ostream& out, const anch::rest::MediaType& contentType, anch::rest::Side side);

    /*!
     * Get body of a value mapped with its \ref FormMapper as a stream
     *
     * \tparam T the value type
     *
     * \param value the value
     * \param contentType the body media type (with its boundary)
     * \param side the side which writes value
     *
     * \return the body stream and its size
     */
    template<anch::rest::FormMapped T>
    static anch::rest::BodyStream stream(const T& value, const anch::rest::MediaType& contentType, anch::rest::Side side);

    /*!
     * Read value mapped with its \ref FormMapper
     *
     * \tparam T the value type
     *
     * \param value the value to fill
     * \param input the input stream
     * \param contentType the body media type (with its boundary)
     * \param side the side which reads value
     */
    template<anch::rest::FormMapped T>
    static void deserialize(T& value, std::istream& input, const anch::rest::MediaType& contentType, anch::rest::Side side);

  };

}

#include "rest/impl/multipartCodec.hpp"
