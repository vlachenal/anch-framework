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

#include <map>
#include <functional>

#include "rest/request.hpp"
#include "rest/response.hpp"
#include "rest/endpoint.hpp"
#include "rest/endpointFunction.hpp"


namespace anch::rest {

  /*!
   * \brief RESTful router
   *
   * HTTP RESTful router which select function to execute according to its media types, HTTP verb and path.\n
   * To selected the endpoint to execute:
   *   -# get matching filters and throw 404 (not found) when no match
   *   -# filter on HTTP verb and throw 405 (method not allowed) when no match
   *   -# filter on content type header and throw 415 (unsupported media type) when no match
   *   -# filter on accept header and throw 406 (not acceptable) when no match
   *   -# execute first endpoint function (which are sorted in map)
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class Router {

    // Attributes +
  private:
    /*! Endpoints/functions mapping */
    std::map<EndPoint, EndpointFunc*> _routes;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref Router default constructor
     */
    Router();
    // Constructors -

    // Destructor +
  public:
    virtual ~Router();
    // Destructor -

    // Methods +
  public:
    /*!
     * Register new endpoint
     *
     * \param func the function to execute for this endpoint
     * \param path the request path
     * \param verb the HTTP verb
     * \param consumes the accepted input media types
     * \param produces the accepted output media types
     *
     * \return \c this
     *
     * \throw anch::rest::RouterException Endpoint already exists
     */
    template<typename... P>
    Router& add(std::function<anch::rest::Response(const anch::rest::Request&, P...)> func,
		const std::string& path,
		const std::string& verb,
		const std::vector<std::string>& consumes,
		const std::vector<std::string>& produces);

    /*!
     * Register new endpoint
     *
     * \param endpoint the \ref EndPoint
     * \param func the function to execute for this endpoint
     *
     * \return \c this
     *
     * \throw anch::rest::RouterException Endpoint already exists
     */
    template<typename... P>
    Router& add(anch::rest::EndPoint& endpoint, std::function<anch::rest::Response(const anch::rest::Request&, P...)> func);

    /*!
     * Execute request
     *
     * \param request the HTTP request
     *
     * \return the HTTP response
     */
    anch::rest::Response execute(anch::rest::Request& request) noexcept;
    // Methods -

  };

} // anch::rest

#include "rest/impl/router.hpp"
