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

#include "anch-rest-server-export.hpp"

#include <functional>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "rest/cors.hpp"
#include "rest/endpoint.hpp"
#include "rest/filter.hpp"
#include "rest/method.hpp"
#include "rest/request.hpp"
#include "rest/response.hpp"

namespace anch::rest {

  /*!
   * REST API: endpoints declaration and routing, independent of the HTTP server.\n
   * \n
   * Endpoints are declared with a method and a path pattern where path parameters are named between
   * braces (\c /pets/{id} ). Empty path segments are ignored (\c /pets/ is \c /pets ). On routing, a
   * literal segment has priority over a parameter.\n
   * \snippet{trimleft} anch-rest/examples/server.cpp api
   * Endpoint arguments and results are described in \ref Endpoint .\n
   * \n
   * \ref handle never throws: an \ref HttpException is answered with its status, any other exception
   * with \c 500 . No route for path is \c 404 , no route for method is \c 405 (with \c Allow header).
   * \c HEAD requests are answered by \c GET endpoints without body. \c OPTIONS requests without
   * \c OPTIONS endpoint are answered with \c 204 and the \c Allow header ( \c OPTIONS \c * with
   * \c 204 ).\n
   * \n
   * Cross-Origin Resource Sharing is enabled with \ref cors : preflight requests are answered before
   * filters (they have no credentials), and CORS headers are added to every response, errors
   * included.\n
   * \n
   * Routes and filters have to be declared before handling requests; \ref handle can then be
   * called from several threads.
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  class ANCH_REST_SERVER_EXPORT Api {

    /*! Routing tree node */
    struct Node;

    /*! Node matching request path, with path parameter values */
    struct Match;

    // Attributes +
  private:
    /*! Routing tree root */
    std::unique_ptr<Node> _root;

    /*! Filters, in declaration order */
    std::vector<anch::rest::Filter> _filters;

    /*! CORS options (disabled when empty) */
    std::optional<anch::rest::CorsOptions> _cors;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref Api constructor
     */
    Api();

    /*! Prohibit \ref Api copy */
    Api(const Api&) = delete;

    /*!
     * \ref Api move constructor
     *
     * \param other the API to move
     */
    Api(Api&& other) noexcept;
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref Api destructor
     */
    virtual ~Api();
    // Destructor -

    // Methods +
  public:
    /*!
     * Declare endpoint
     *
     * \param method the method
     * \param pattern the path pattern
     * \param function the endpoint function (see \ref Endpoint )
     *
     * \return the endpoint, to declare its codecs
     *
     * \throw std::invalid_argument when pattern is invalid, when the number of path parameters does
     *        not match function arguments or when route already exists
     */
    template<typename F>
    anch::rest::Endpoint<std::decay_t<F>>& route(anch::rest::Method method, std::string_view pattern, F&& function);

    /*!
     * Declare \c GET endpoint (see \ref route )
     *
     * \param pattern the path pattern
     * \param function the endpoint function
     *
     * \return the endpoint
     */
    template<typename F>
    anch::rest::Endpoint<std::decay_t<F>>& get(std::string_view pattern, F&& function);

    /*!
     * Declare \c POST endpoint (see \ref route )
     *
     * \param pattern the path pattern
     * \param function the endpoint function
     *
     * \return the endpoint
     */
    template<typename F>
    anch::rest::Endpoint<std::decay_t<F>>& post(std::string_view pattern, F&& function);

    /*!
     * Declare \c PUT endpoint (see \ref route )
     *
     * \param pattern the path pattern
     * \param function the endpoint function
     *
     * \return the endpoint
     */
    template<typename F>
    anch::rest::Endpoint<std::decay_t<F>>& put(std::string_view pattern, F&& function);

    /*!
     * Declare \c PATCH endpoint (see \ref route )
     *
     * \param pattern the path pattern
     * \param function the endpoint function
     *
     * \return the endpoint
     */
    template<typename F>
    anch::rest::Endpoint<std::decay_t<F>>& patch(std::string_view pattern, F&& function);

    /*!
     * Declare \c DELETE endpoint (see \ref route )
     *
     * \param pattern the path pattern
     * \param function the endpoint function
     *
     * \return the endpoint
     */
    template<typename F>
    anch::rest::Endpoint<std::decay_t<F>>& del(std::string_view pattern, F&& function);

    /*!
     * Add filter. Filters are executed in declaration order around endpoints (and around routing
     * errors).
     *
     * \param filter the filter
     *
     * \return \c this
     */
    Api& filter(anch::rest::Filter filter);

    /*!
     * Enable Cross-Origin Resource Sharing (CORS), see \ref CorsOptions .\n
     * Preflight requests ( \c OPTIONS with \c Origin and \c Access-Control-Request-Method headers)
     * are answered without executing filters: \c 204 with \c Access-Control-Allow-* headers, \c 403
     * when origin, method or a requested header is not allowed, \c 404 when path is unknown. Other
     * responses get \c Access-Control-Allow-Origin (and \c Access-Control-Expose-Headers ,
     * \c Access-Control-Allow-Credentials ) when their origin is allowed.
     *
     * \param options the CORS options
     *
     * \return \c this
     *
     * \throw std::invalid_argument when there is no origin or when \c "*" origin is used with
     *        credentials
     */
    Api& cors(anch::rest::CorsOptions options);

    /*!
     * Check every endpoint configuration (codecs). Server adapters call it at startup.
     *
     * \throw std::logic_error listing invalid endpoints
     */
    void validate() const;

    /*!
     * Handle request
     *
     * \param request the request
     *
     * \return the response
     */
    anch::rest::Response handle(anch::rest::Request& request) noexcept;

    /*! Prohibit \ref Api copy */
    Api& operator=(const Api&) = delete;

    /*!
     * \ref Api move assignment
     *
     * \param other the API to move
     *
     * \return \c this
     */
    Api& operator=(Api&& other) noexcept;

  private:
    /*!
     * Parse path pattern
     *
     * \param pattern the path pattern
     *
     * \return the path parameter names
     *
     * \throw std::invalid_argument when pattern is invalid
     */
    static std::vector<std::string> parsePattern(std::string_view pattern);

    /*!
     * Register route in routing tree
     *
     * \param route the route
     *
     * \throw std::invalid_argument when route already exists or when path parameters do not match
     */
    void addRoute(std::unique_ptr<anch::rest::Route> route);

    /*!
     * Find routing tree nodes matching request path: literal segments first, then parameters
     *
     * \param request the request
     *
     * \return the matching nodes
     *
     * \throw HttpException \c 400 when path encoding is invalid
     */
    std::vector<Match> match(const anch::rest::Request& request) const;

    /*!
     * Get methods allowed on matching nodes ( \c HEAD with \c GET , and \c OPTIONS )
     *
     * \param matches the matching nodes
     *
     * \return the allowed methods
     */
    static std::set<anch::rest::Method> allowedMethods(const std::vector<Match>& matches);

    /*!
     * Answer CORS preflight request
     *
     * \param request the request
     *
     * \return the response
     *
     * \throw HttpException \c 404 when path is unknown
     */
    anch::rest::Response preflight(const anch::rest::Request& request) const;

    /*!
     * Add CORS headers to response when CORS is enabled and request origin is allowed
     *
     * \param request the request
     * \param response the response
     */
    void addCorsHeaders(const anch::rest::Request& request, anch::rest::Response& response) const;

    /*!
     * Build error response (with CORS headers)
     *
     * \param request the request
     * \param status the error status
     * \param message the error message, body when not empty
     *
     * \return the response
     */
    anch::rest::Response error(const anch::rest::Request& request, anch::rest::Status status, const char* message) const noexcept;

    /*!
     * Route request to endpoint
     *
     * \param request the request
     *
     * \return the response
     */
    anch::rest::Response dispatch(anch::rest::Request& request);

    /*!
     * Execute filters from index then dispatch request
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

#include "rest/impl/api.hpp"
