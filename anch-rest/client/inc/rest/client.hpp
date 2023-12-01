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

#include <string>
#include <optional>

#include "rest/clientIface.hpp"
#include "rest/request.hpp"


namespace anch::rest {

  class WebClient {
    // Attributes +
  private:
    /*! HTTP client implementation */
    anch::rest::ClientIface& _client;

    /*! Optional base path to prepend to request path */
    std::optional<std::string> _basePath;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref WebClient constructor
     *
     * \param client the HTTP client implementation to use
     * \param basePath the optional base path
     */
    WebClient(anch::rest::ClientIface& client, std::optional<std::string> basePath = std::optional<std::string>());
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref WebClient destructor
     */
    virtual ~WebClient();
    // Destructor -

    // Methods +
  public:
    /*!
     * Map path parameters and execute GET HTTP method
     *
     * \tparam P the path parameters types
     *
     * \param request the request
     * \param params the path parameters
     *
     * \return return the response
     */
    template<typename... P>
    anch::rest::Response
    GET(anch::rest::Request& request, P... params);

    /*!
     * Execute GET HTTP method
     *
     * \tparam P the path parameters types
     *
     * \param path the path
     * \param params the path parameters
     *
     * \return return the response
     */
    template<typename... P>
    anch::rest::Response
    GET(const std::string& path, P... params);

    /*!
     * Map path parameters and execute POST HTTP method
     *
     * \tparam P the path parameters types
     *
     * \param request the request
     * \param params the path parameters
     *
     * \return return the response
     */
    template<typename... P>
    anch::rest::Response
    POST(anch::rest::Request& request, P... params);

    /*!
     * Map path parameters and execute PUT HTTP method
     *
     * \tparam P the path parameters types
     *
     * \param request the request
     * \param params the path parameters
     *
     * \return return the response
     */
    template<typename... P>
    anch::rest::Response
    PUT(anch::rest::Request& request, P... params);

    /*!
     * Map path parameters and execute PATCH HTTP method
     *
     * \tparam P the path parameters types
     *
     * \param request the request
     * \param params the path parameters
     *
     * \return return the response
     */
    template<typename... P>
    anch::rest::Response
    PATCH(anch::rest::Request& request, P... params);

    /*!
     * Map path parameters and execute DELETE HTTP method
     *
     * \tparam P the path parameters types
     *
     * \param request the request
     * \param params the path parameters
     *
     * \return return the response
     */
    template<typename... P>
    anch::rest::Response
    DELETE(anch::rest::Request& request, P... params);

    /*!
     * Map path parameters and execute DELETE HTTP method
     *
     * \tparam P the path parameters types
     *
     * \param path the path
     * \param params the path parameters
     *
     * \return return the response
     */
    template<typename... P>
    anch::rest::Response
    DELETE(const std::string& path, P... params);

    /*!
     * Map path parameters and execute HEAD HTTP method
     *
     * \tparam P the path parameters types
     *
     * \param request the request
     * \param params the path parameters
     *
     * \return return the response
     */
    template<typename... P>
    anch::rest::Response
    HEAD(anch::rest::Request& request, P... params);

    /*!
     * Map path parameters and execute HEAD HTTP method
     *
     * \tparam P the path parameters types
     *
     * \param path the path
     * \param params the path parameters
     *
     * \return return the response
     */
    template<typename... P>
    anch::rest::Response
    HEAD(const std::string& path, P... params);

    /*!
     * Map path parameters and execute OPTIONS HTTP method
     *
     * \tparam P the path parameters types
     *
     * \param request the request
     * \param params the path parameters
     *
     * \return return the response
     */
    template<typename... P>
    anch::rest::Response
    OPTIONS(anch::rest::Request& request, P... params);

    /*!
     * Map path parameters and execute OPTIONS HTTP method
     *
     * \tparam P the path parameters types
     *
     * \param path the path
     * \param params the path parameters
     *
     * \return return the response
     */
    template<typename... P>
    anch::rest::Response
    OPTIONS(const std::string& path, P... params);

    /*!
     * Map path parameters and execute CONNECT HTTP method
     *
     * \tparam P the path parameters types
     *
     * \param request the request
     * \param params the path parameters
     *
     * \return return the response
     */
    template<typename... P>
    anch::rest::Response
    CONNECT(anch::rest::Request& request, P... params);

    /*!
     * Map path parameters and execute CONNECT HTTP method
     *
     * \tparam P the path parameters types
     *
     * \param path the path
     * \param params the path parameters
     *
     * \return return the response
     */
    template<typename... P>
    anch::rest::Response
    CONNECT(const std::string& path, P... params);

    /*!
     * Map path parameters and execute TRACE HTTP method
     *
     * \tparam P the path parameters types
     *
     * \param path the path
     * \param params the path parameters
     *
     * \return return the response
     */
    template<typename... P>
    anch::rest::Response
    TRACE(const std::string& path, P... params);
    // Methods -

  };

}  // anch::rest

#include "rest/impl/client.hpp"
