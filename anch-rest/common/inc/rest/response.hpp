/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
  Copyright (C) 2012 Vincent Lachenal

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

#include <functional>
#include <ostream>
#include <string>
#include <string_view>

#include "rest/headers.hpp"
#include "rest/status.hpp"

namespace anch::rest {

  /*! Response body writer: it is called by the server adapter to write the body */
  using BodyWriter = std::function<void(std::ostream&)>;

  /*!
   * HTTP response.\n
   * The body is written by a function when the response is sent, which allows streaming.\n
   * Setters return the response to chain them, on lvalues and on temporaries:
   * \code{.cpp}
return anch::rest::Response(anch::rest::Status::CREATED).header(anch::rest::header::LOCATION, "/pets/1");
   * \endcode
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class Response {

    // Attributes +
  private:
    /*! Status */
    anch::rest::Status _status;

    /*! Headers */
    anch::rest::Headers _headers;

    /*! Body writer */
    anch::rest::BodyWriter _body;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref Response constructor
     *
     * \param status the status
     */
    Response(anch::rest::Status status = anch::rest::Status::OK);
    // Constructors -

    // Methods +
  public:
    /*!
     * Set header value
     *
     * \param name the header name
     * \param value the header value
     *
     * \return \c this
     */
    Response& header(std::string_view name, std::string_view value) &;

    /*!
     * Set header value
     *
     * \param name the header name
     * \param value the header value
     *
     * \return \c this
     */
    Response&& header(std::string_view name, std::string_view value) &&;

    /*!
     * Set \c Content-Type header
     *
     * \param mediaType the media type
     *
     * \return \c this
     */
    Response& contentType(std::string_view mediaType) &;

    /*!
     * Set \c Content-Type header
     *
     * \param mediaType the media type
     *
     * \return \c this
     */
    Response&& contentType(std::string_view mediaType) &&;

    /*!
     * Set body content
     *
     * \param content the body content
     *
     * \return \c this
     */
    Response& body(std::string content) &;

    /*!
     * Set body content
     *
     * \param content the body content
     *
     * \return \c this
     */
    Response&& body(std::string content) &&;

    /*!
     * Set body writer
     *
     * \param writer the function which writes the body
     *
     * \return \c this
     */
    Response& stream(anch::rest::BodyWriter writer) &;

    /*!
     * Set body writer
     *
     * \param writer the function which writes the body
     *
     * \return \c this
     */
    Response&& stream(anch::rest::BodyWriter writer) &&;

    /*!
     * Check if response has a body
     *
     * \return \c true when response has a body
     */
    bool hasBody() const noexcept;

    /*!
     * Write body. Nothing is written when there is no body.
     *
     * \param out the output stream
     */
    void writeBody(std::ostream& out) const;

    /*!
     * Get body content (body is written in a string)
     *
     * \return the body content, empty when there is no body
     */
    std::string content() const;

    /*!
     * Remove body (headers are kept)
     */
    void clearBody() noexcept;
    // Methods -

    // Accessors +
  public:
    /*!
     * Status getter
     *
     * \return the status
     */
    anch::rest::Status getStatus() const noexcept;

    /*!
     * Status setter
     *
     * \param status the status
     */
    void setStatus(anch::rest::Status status) noexcept;

    /*!
     * Headers getter
     *
     * \return the headers
     */
    anch::rest::Headers& getHeaders() noexcept;

    /*!
     * Headers getter
     *
     * \return the headers
     */
    const anch::rest::Headers& getHeaders() const noexcept;
    // Accessors -

  };

}
