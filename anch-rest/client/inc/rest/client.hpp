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

#include <concepts>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "rest/codec.hpp"
#include "rest/filter.hpp"
#include "rest/headers.hpp"
#include "rest/httpError.hpp"
#include "rest/method.hpp"
#include "rest/request.hpp"
#include "rest/response.hpp"
#include "rest/transport.hpp"

namespace anch::rest {

  /*!
   * Value which can be used as path or query parameter: strings, numbers, \c bool and types with
   * stream injection operator
   *
   * \tparam T the value type
   *
   * \since 0.1
   */
  template<typename T>
  concept ParameterValue = std::convertible_to<const T&, std::string_view>
    || std::is_arithmetic_v<T>
    || requires(std::ostream& out, const T& value) { out << value; };

  /*!
   * Format parameter value
   *
   * \param value the value
   *
   * \return the formatted value (not encoded)
   *
   * \since 0.1
   */
  template<anch::rest::ParameterValue T>
  std::string toParameterString(const T& value);

  class Client;

  /*!
   * Request being built by \ref Client .\n
   * Accepted codecs are part of the type: \ref accept adds a codec and \ref as deserializes the response
   * with the accepted codec matching its \c Content-Type :
   * \code{.cpp}
Pet pet = client.get("/pets/{id}", 42).accept<anch::rest::Json>().as<Pet>();
   * \endcode
   * A call is sent once: \ref send , \ref execute and \ref as are called on rvalues.
   *
   * \tparam Codecs the accepted codecs
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  template<typename... Codecs>
  class Call {

    /*! Calls with other codecs are friends (\ref accept ) */
    template<typename...>
    friend class Call;

    // Attributes +
  private:
    /*! The client */
    anch::rest::Client* _client;

    /*! The request */
    anch::rest::Request _request;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref Call constructor
     *
     * \param client the client
     * \param request the request
     */
    Call(anch::rest::Client& client, anch::rest::Request request);
    // Constructors -

    // Methods +
  public:
    /*!
     * Set request header
     *
     * \param name the header name
     * \param value the header value
     *
     * \return \c this
     */
    Call& header(std::string_view name, std::string_view value) &;

    /*!
     * Set request header
     *
     * \param name the header name
     * \param value the header value
     *
     * \return \c this
     */
    Call&& header(std::string_view name, std::string_view value) &&;

    /*!
     * Add query parameter (it is encoded)
     *
     * \param name the parameter name
     * \param value the parameter value
     *
     * \return \c this
     */
    template<anch::rest::ParameterValue T>
    Call& query(std::string_view name, const T& value) &;

    /*!
     * Add query parameter (it is encoded)
     *
     * \param name the parameter name
     * \param value the parameter value
     *
     * \return \c this
     */
    template<anch::rest::ParameterValue T>
    Call&& query(std::string_view name, const T& value) &&;

    /*!
     * Accept codec media type (added to \c Accept header)
     *
     * \tparam C the codec
     *
     * \return the call with this codec
     */
    template<typename C>
    Call<Codecs..., C> accept() &&;

    /*!
     * Set body serialized with codec (\c Content-Type is the codec media type)
     *
     * \tparam C the codec
     *
     * \param value the body value
     *
     * \return \c this
     */
    template<typename C, typename T>
    Call& body(const T& value) &;

    /*!
     * Set body serialized with codec (\c Content-Type is the codec media type)
     *
     * \tparam C the codec
     *
     * \param value the body value
     *
     * \return \c this
     */
    template<typename C, typename T>
    Call&& body(const T& value) &&;

    /*!
     * Set body content
     *
     * \param content the body content
     * \param contentType the content type
     *
     * \return \c this
     */
    Call& body(std::string content, std::string_view contentType) &;

    /*!
     * Set body content
     *
     * \param content the body content
     * \param contentType the content type
     *
     * \return \c this
     */
    Call&& body(std::string content, std::string_view contentType) &&;

    /*!
     * Send request
     *
     * \return the response, whatever its status
     *
     * \throw anch::rest::TransportError when request fails
     */
    anch::rest::Response send() &&;

    /*!
     * Send request and check response status
     *
     * \throw anch::rest::HttpError when status is an error (>= 400)
     * \throw anch::rest::TransportError when request fails
     */
    void execute() &&;

    /*!
     * Send request and deserialize response with the accepted codec matching its \c Content-Type
     *
     * \tparam T the result type (default constructible)
     *
     * \return the response value
     *
     * \throw anch::rest::HttpError when status is an error (>= 400)
     * \throw anch::rest::TransportError when request fails
     * \throw std::runtime_error when response content type is not accepted
     * \throw std::exception codec errors
     */
    template<typename T>
    T as() &&;

    /*!
     * Request getter
     *
     * \return the request
     */
    anch::rest::Request& getRequest() noexcept;
    // Methods -

  };

  /*!
   * HTTP client: builds requests and sends them through a \ref Transport .\n
   * \code{.cpp}
anch::rest::Client client(std::make_unique<anch::rest::CurlTransport>(), "https://api.example.com/v1");
client.filter(anch::rest::bearerAuth(token));
Pet pet = client.get("/pets/{id}", 42).accept<anch::rest::Json>().as<Pet>();
anch::rest::Response res = client.post("/pets").body<anch::rest::Json>(pet).send();
   * \endcode
   * Path parameters (\c {name} placeholders) are replaced in order and percent-encoded.\n
   * Default headers (\c User-Agent is \c anch-rest ) are set when the request does not set them, then
   * filters are executed in declaration order before the transport.\n
   * A client is as thread safe as its transport.
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class Client {

    // Attributes +
  private:
    /*! Transport */
    std::unique_ptr<anch::rest::Transport> _transport;

    /*! Base URL, without trailing slash */
    std::string _baseUrl;

    /*! Default headers */
    anch::rest::Headers _headers;

    /*! Filters */
    std::vector<anch::rest::Filter> _filters;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref Client constructor
     *
     * \param transport the transport
     * \param baseUrl the base URL (\c https://api.example.com/v1 )
     */
    Client(std::unique_ptr<anch::rest::Transport> transport, std::string_view baseUrl);
    // Constructors -

    // Methods +
  public:
    /*!
     * Set default header
     *
     * \param name the header name
     * \param value the header value
     *
     * \return \c this
     */
    Client& header(std::string_view name, std::string_view value);

    /*!
     * Add filter
     *
     * \param filter the filter
     *
     * \return \c this
     */
    Client& filter(anch::rest::Filter filter);

    /*!
     * Start a request
     *
     * \param method the method
     * \param path the path relative to base URL, with \c {name} placeholders
     * \param parameters the path parameters, in placeholders order
     *
     * \return the call
     *
     * \throw std::invalid_argument when the number of parameters does not match placeholders
     */
    template<anch::rest::ParameterValue... P>
    anch::rest::Call<> request(anch::rest::Method method, std::string_view path, const P&... parameters);

    /*!
     * Start a \c GET request (see \ref request )
     *
     * \param path the path
     * \param parameters the path parameters
     *
     * \return the call
     */
    template<anch::rest::ParameterValue... P>
    anch::rest::Call<> get(std::string_view path, const P&... parameters);

    /*!
     * Start a \c POST request (see \ref request )
     *
     * \param path the path
     * \param parameters the path parameters
     *
     * \return the call
     */
    template<anch::rest::ParameterValue... P>
    anch::rest::Call<> post(std::string_view path, const P&... parameters);

    /*!
     * Start a \c PUT request (see \ref request )
     *
     * \param path the path
     * \param parameters the path parameters
     *
     * \return the call
     */
    template<anch::rest::ParameterValue... P>
    anch::rest::Call<> put(std::string_view path, const P&... parameters);

    /*!
     * Start a \c PATCH request (see \ref request )
     *
     * \param path the path
     * \param parameters the path parameters
     *
     * \return the call
     */
    template<anch::rest::ParameterValue... P>
    anch::rest::Call<> patch(std::string_view path, const P&... parameters);

    /*!
     * Start a \c DELETE request (see \ref request )
     *
     * \param path the path
     * \param parameters the path parameters
     *
     * \return the call
     */
    template<anch::rest::ParameterValue... P>
    anch::rest::Call<> del(std::string_view path, const P&... parameters);

    /*!
     * Start a \c HEAD request (see \ref request )
     *
     * \param path the path
     * \param parameters the path parameters
     *
     * \return the call
     */
    template<anch::rest::ParameterValue... P>
    anch::rest::Call<> head(std::string_view path, const P&... parameters);

    /*!
     * Send request: default headers, filters, then transport
     *
     * \param request the request
     *
     * \return the response
     *
     * \throw anch::rest::TransportError when request fails
     */
    anch::rest::Response send(anch::rest::Request& request);

    /*!
     * Get request URL: base URL, path and encoded query parameters
     *
     * \param request the request
     *
     * \return the URL
     */
    std::string url(const anch::rest::Request& request) const;

    /*!
     * Replace path placeholders (\c {name} ) with percent-encoded parameters
     *
     * \param path the path
     * \param parameters the parameters, in placeholders order
     *
     * \return the path
     *
     * \throw std::invalid_argument when the number of parameters does not match placeholders
     */
    static std::string formatPath(std::string_view path, const std::vector<std::string>& parameters);

  private:
    /*!
     * Execute filters from index then transport
     *
     * \param request the request
     * \param index the filter index
     *
     * \return the response
     */
    anch::rest::Response next(anch::rest::Request& request, std::size_t index);
    // Methods -

  };

}

#include "rest/impl/client.hpp"
