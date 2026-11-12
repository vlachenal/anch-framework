/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
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
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

#include <istream>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "rest/headers.hpp"
#include "rest/mediaType.hpp"
#include "rest/method.hpp"

namespace anch::rest {

  /*!
   * HTTP request.\n
   * The path is kept percent-encoded (path parameters are decoded by the router); query parameters
   * are decoded.\n
   * The request owns its body stream: it can be moved, not copied.
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  class Request {

    // Attributes +
  private:
    /*! Method */
    anch::rest::Method _method;

    /*! Path (percent-encoded) */
    std::string _path;

    /*! Query parameters (decoded), in request order */
    std::vector<std::pair<std::string, std::string>> _query;

    /*! Headers */
    anch::rest::Headers _headers;

    /*! Body */
    std::unique_ptr<std::istream> _body;

    /*! Path parameters (decoded), set by router */
    std::map<std::string, std::string, std::less<>> _pathParameters;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref Request default constructor: \c GET \c /
     */
    Request();

    /*!
     * \ref Request constructor
     *
     * \param method the method
     * \param target the request target (path and query, \c /pets?limit=10 )
     */
    Request(anch::rest::Method method, std::string_view target);
    // Constructors -

    // Methods +
  public:
    /*!
     * Set path and query parameters from request target (\c /pets?limit=10 )
     *
     * \param target the request target
     */
    void setTarget(std::string_view target);

    /*!
     * Get query parameter first value
     *
     * \param name the parameter name
     *
     * \return the value, empty when parameter is not set
     */
    std::optional<std::string_view> getQueryParameter(std::string_view name) const;

    /*!
     * Add a query parameter
     *
     * \param name the parameter name
     * \param value the parameter value
     */
    void addQueryParameter(std::string_view name, std::string_view value);

    /*!
     * Get path parameter value
     *
     * \param name the parameter name
     *
     * \return the value, empty when parameter does not exist
     */
    std::optional<std::string_view> getPathParameter(std::string_view name) const;

    /*!
     * Get \c Content-Type header
     *
     * \return the content type, empty when header is not set or invalid
     */
    std::optional<anch::rest::MediaType> getContentType() const;

    /*!
     * Get \c Accept header media ranges (see \ref MediaType::parseAccept )
     *
     * \return the media ranges, empty when header is not set
     */
    std::vector<anch::rest::MediaType> getAccept() const;

    /*!
     * Check if request has a body
     *
     * \return \c true when request has a body
     */
    bool hasBody() const noexcept;

    /*!
     * Get body stream. Request must have a body (\ref hasBody ).
     *
     * \return the body stream
     */
    std::istream& getBody() const;

    /*!
     * Set body stream
     *
     * \param body the body stream
     */
    void setBody(std::unique_ptr<std::istream> body);

    /*!
     * Set body content
     *
     * \param body the body content
     */
    void setBody(std::string body);
    // Methods -

    // Accessors +
  public:
    /*!
     * Method getter
     *
     * \return the method
     */
    anch::rest::Method getMethod() const noexcept;

    /*!
     * Method setter
     *
     * \param method the method
     */
    void setMethod(anch::rest::Method method) noexcept;

    /*!
     * Path getter
     *
     * \return the percent-encoded path
     */
    const std::string& getPath() const noexcept;

    /*!
     * Path setter
     *
     * \param path the percent-encoded path
     */
    void setPath(std::string_view path);

    /*!
     * Query parameters getter
     *
     * \return the decoded query parameters
     */
    const std::vector<std::pair<std::string, std::string>>& getQueryParameters() const noexcept;

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

    /*!
     * Path parameters getter
     *
     * \return the path parameters
     */
    const std::map<std::string, std::string, std::less<>>& getPathParameters() const noexcept;

    /*!
     * Path parameters setter
     *
     * \param parameters the path parameters
     */
    void setPathParameters(std::map<std::string, std::string, std::less<>> parameters);
    // Accessors -

  };

  /*!
   * Decode percent-encoded string
   *
   * \param value the value to decode
   * \param plusAsSpace decode \c + as space (query strings)
   *
   * \return the decoded value, empty when value is invalid
   */
  std::optional<std::string> percentDecode(std::string_view value, bool plusAsSpace = false);

  /*!
   * Percent-encode string (RFC 3986 unreserved characters are kept)
   *
   * \param value the value to encode
   *
   * \return the encoded value
   */
  std::string percentEncode(std::string_view value);

}
