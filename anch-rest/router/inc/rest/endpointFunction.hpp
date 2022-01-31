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

#include <functional>
#include <concepts>

#include "rest/constants.hpp"
#include "rest/request.hpp"
#include "rest/response.hpp"


namespace anch::rest {

  /*!
   * \brief Endpoint function interface
   *
   * Endpoint function executor interface
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class EndpointFunc {
  public:
    // Destructor +
    /*!
     * \ref EndpointFunc destructor
     */
    virtual ~EndpointFunc();
    // Destructor -

    // Methods +
    /*!
     * Execute endpoint function
     *
     * \param request the HTTP request
     *
     * \return the response
     */
    virtual anch::rest::Response execute(const anch::rest::Request& request) = 0;
    // Methods -
  };

  /*!
   * \ref EndpointFunc implementation
   *
   * Manage endpoint execution without any path parameter nor body (useful for GET and DELETE HTTP requests).
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class EndpointSimpleFunc: public EndpointFunc {

    // Attributes +
  private:
    /*! Function to execute */
    std::function<anch::rest::Response(const anch::rest::Request&)> _func;
    // Attributes -

    // Constructors +
  public:
    /*!
     * Forbids \ref EndpointSimpleFunc default constructor
     */
    EndpointSimpleFunc() = delete;

    /*!
     * \ref EndpointSimpleFunc constructor
     *
     * \param func the function to execute
     */
    EndpointSimpleFunc(std::function<anch::rest::Response(const anch::rest::Request&)> func);
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref EndpointSimpleFunc destructor
     */
    virtual ~EndpointSimpleFunc();
    // Destructor

    // Methods +
  public:
    /*!
     * Execute request with the underlying function
     *
     * \param request the request to execute
     *
     * \return the response
     */
    virtual anch::rest::Response execute(const anch::rest::Request& request) override;
    // Methods -

  };

  /*!
   * \ref EndpointFunc implementation
   *
   * Manage endpoint execution with path parameter(s) or body
   *
   * \tparam T the parameter types
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  template<typename... T>
  class EndpointParamsFunc: public EndpointFunc {

    // Attributes +
  private:
    /*! Function to bind and execute */
    std::function<anch::rest::Response(const anch::rest::Request&, T...)> _func;
    // Attributes -

    // Constructors +
  public:
    /*!
     * Forbids \ref EndpointParamsFunc default constructor
     */
    EndpointParamsFunc() = delete;

    /*!
     * \ref EndpointParamsFunc constructor
     *
     * \param func the function to execute for this endpoint
     */
    EndpointParamsFunc(std::function<anch::rest::Response(const anch::rest::Request&, T...)> func);
    // Constructors -

    // Destructors +
  public:
    /*!
     * \ref EndpointParamsFunc destructor
     */
    virtual ~EndpointParamsFunc();
    // Destructors -

  public:
    // Methods +
    /*!
     * Bind parameters and execute HTTP request
     *
     * \param request the request
     *
     * \return the response
     */
    virtual anch::rest::Response execute(const anch::rest::Request& request) override;
    // Methods -
  };

  /*!
   * Convert path parameter into selected type.\n
   * When conversion fails, it will raise 400.
   *
   * \param param the path parameter to convert
   *
   * \return the converted parameter
   *
   * \throw int 400 when conversion fails
   */
  template<typename T>
  T convertPathParam(const std::string& param);

} // anch::rest

#include "rest/impl/endpointFunction.hpp"
